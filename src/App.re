type state = {
  savedTasks: list(Types.task),
  tasks: list(Types.task),
  requestStatus: Types.requestStatus,
  isLoading: bool,
  isSaving: bool,
  alerts: list(Types.alert)
};

let alertReducer = (action: Actions.alertAction, state) =>
  switch action {
  | Actions.ShowAlert(content) => [
      ({id: [%bs.raw {| (new Date()).getTime() |}], content}: Types.alert),
      ...state
    ]
  | Actions.CloseAlert(id) =>
    List.filter((alert: Types.alert) => alert.id != id, state)
  };

let renderTaskItem = (self: ReasonReact.self('a, 'b, 'c), task: Types.task) =>
  <TaskItem
    key=(string_of_float(task.id))
    title=task.title
    onRemove=((_) => self.send(Actions.RemoveTask(task.id)))
    onChange=(
      (e) =>
        self.send(
          Actions.UpdateTask(
            task.id,
            ReactDOMRe.domElementToObj(ReactEventRe.Form.target(e))##value
          )
        )
    )
  />;

let renderAlert = (self: ReasonReact.self('a, 'b, 'c), alert) =>
  <Alert
    alert
    onHide=(
      (_event) => self.send(Actions.AlertAction(Actions.CloseAlert(alert.id)))
    )
  />;

let saveDisabled = (state) =>
  Js.Boolean.to_js_boolean(
    state.isLoading || state.isSaving || state.tasks == state.savedTasks
  );

let component = ReasonReact.reducerComponent("App");

let make = (_children) => {
  ...component,
  initialState: () => {
    tasks: [],
    savedTasks: [],
    requestStatus: Done,
    alerts: [],
    isLoading: true,
    isSaving: false
  },
  reducer: (action: Actions.action, state) =>
    switch action {
    | Actions.AlertAction(action) =>
      ReasonReact.Update({
        ...state,
        alerts: alertReducer(action, state.alerts)
      })
    | Actions.AddTask =>
      ReasonReact.Update({
        ...state,
        tasks: [
          {id: [%bs.raw {| (new Date()).getTime() |}], title: ""},
          ...state.tasks
        ]
      })
    | Actions.UpdateTask(id, title) =>
      ReasonReact.Update({
        ...state,
        tasks:
          List.map(
            (t: Types.task) => t.id == id ? {...t, title} : t,
            state.tasks
          )
      })
    | Actions.RemoveTask(id) =>
      ReasonReact.Update({
        ...state,
        tasks: List.filter((task: Types.task) => task.id != id, state.tasks)
      })
    | Actions.SaveTasks =>
      ReasonReact.UpdateWithSideEffects(
        {...state, isSaving: true},
        (
          (self) =>
            Api.onSave(
              self.state.tasks,
              (r) => self.send(Actions.TasksSaveResponse(r))
            )
        )
      )
    | Actions.TasksLoadResponse(json) =>
      switch (Api.decodeTasks(json)) {
      | Some(tasks) =>
        ReasonReact.Update({
          ...state,
          tasks,
          savedTasks: tasks,
          isLoading: false
        })
      | None => ReasonReact.NoUpdate
      | exception (Json.Decode.DecodeError(_messsage)) =>
        ReasonReact.UpdateWithSideEffects(
          {...state, isLoading: false},
          (
            (self) =>
              self.send(
                Actions.AlertAction(
                  Actions.ShowAlert(Error("Couldn't load tasks"))
                )
              )
          )
        )
      }
    | Actions.TasksSaveResponse(response) =>
      if (Fetch.Response.ok(response)) {
        ReasonReact.UpdateWithSideEffects(
          {...state, savedTasks: state.tasks, isSaving: false},
          (
            (self) =>
              self.send(
                Actions.AlertAction(
                  Actions.ShowAlert(Types.Success("Tasks saved"))
                )
              )
          )
        )
      } else {
        ReasonReact.UpdateWithSideEffects(
          {...state, isSaving: false},
          (
            (self) =>
              self.send(
                Actions.AlertAction(
                  Actions.ShowAlert(Types.Error("Can't save tasks"))
                )
              )
          )
        )
      }
    },
  render: (self) =>
    <div>
      <div className="header" />
      <div className="container">
        <div className="subheader">
          <h1> (ReasonReact.stringToElement("Tasks")) </h1>
          <div className="subheader-buttons">
            <button
              className="button"
              onClick=((_event) => self.send(Actions.AddTask))>
              (ReasonReact.stringToElement("Add Task"))
            </button>
            <button
              className="button button-primary"
              onClick=((_event) => self.send(Actions.SaveTasks))
              disabled=(saveDisabled(self.state))>
              (ReasonReact.stringToElement("Save"))
            </button>
          </div>
        </div>
        (
          ReasonReact.createDomElement(
            "div",
            ~props={"className": ""},
            Array.of_list(List.map(renderTaskItem(self), self.state.tasks))
          )
        )
        (
          ReasonReact.createDomElement(
            "div",
            ~props={"className": "alerts"},
            Array.of_list(List.map(renderAlert(self), self.state.alerts))
          )
        )
      </div>
    </div>,
  didMount: (self) => {
    Api.onLoad((json) => self.send(Actions.TasksLoadResponse(json)));
    ReasonReact.NoUpdate
  }
};
