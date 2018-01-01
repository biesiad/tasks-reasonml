type state = {
  savedTasks: list(Types.task),
  tasks: list(Types.task),
  requestStatus: Types.requestStatus,
  alerts: list(Types.alert),
  isLoading: bool,
  isSaving: bool
};

let newTask = () : Types.task => {
  id: [%bs.raw {| (new Date()).getTime() |}],
  title: ""
};

let removeTask = (id: float, tasks) =>
  List.filter((task: Types.task) => task.id != id, tasks);

let updateTask = (id, title, tasks) =>
  List.map((t: Types.task) => t.id == id ? {...t, title} : t, tasks);

let renderTaskItem = (self: ReasonReact.self('a, 'b, 'c), task: Types.task) =>
  <TaskItem
    key=(string_of_float(task.id))
    title=task.title
    onRemove=(self.reduce((_) => Actions.RemoveTask(task.id)))
    onChange=(
      self.reduce(
        (e) =>
          Actions.UpdateTask(
            task.id,
            ReactDOMRe.domElementToObj(ReactEventRe.Form.target(e))##value
          )
      )
    )
  />;

let newAlert = (content) : Types.alert => {
  id: [%bs.raw {| (new Date()).getTime() |}],
  content
};

let renderAlert = (self: ReasonReact.self('a, 'b, 'c), alert) =>
  <Alert
    alert
    onHide=(self.reduce((_event) => Actions.CloseAlert(alert.id)))
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
  reducer: (action, state) =>
    switch action {
    | Actions.AddTask =>
      ReasonReact.Update({...state, tasks: [newTask(), ...state.tasks]})
    | Actions.UpdateTask(id, title) =>
      ReasonReact.Update({...state, tasks: updateTask(id, title, state.tasks)})
    | Actions.RemoveTask(id) =>
      ReasonReact.Update({...state, tasks: removeTask(id, state.tasks)})
    | Actions.SaveTasks =>
      ReasonReact.UpdateWithSideEffects(
        {...state, isSaving: true},
        (
          (self) =>
            Api.onSave(
              self.state.tasks,
              self.reduce((r) => Actions.TasksSaveResponse(r))
            )
        )
      )
    | Actions.ShowAlert(content) =>
      ReasonReact.Update({
        ...state,
        alerts: [newAlert(content), ...state.alerts]
      })
    | Actions.CloseAlert(id) =>
      ReasonReact.Update({
        ...state,
        alerts:
          List.filter((alert: Types.alert) => alert.id != id, state.alerts)
      })
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
              self.reduce(
                (_) => Actions.ShowAlert(Error("Couldn't load tasks")),
                ()
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
              self.reduce(
                (_) => Actions.ShowAlert(Types.Success("Tasks saved")),
                ()
              )
          )
        )
      } else {
        ReasonReact.UpdateWithSideEffects(
          {...state, isSaving: false},
          (
            (self) =>
              self.reduce(
                (_) => Actions.ShowAlert(Types.Error("Can't save tasks")),
                ()
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
              onClick=(self.reduce((_event) => Actions.AddTask))>
              (ReasonReact.stringToElement("Add Task"))
            </button>
            <button
              className="button button-primary"
              onClick=(self.reduce((_event) => Actions.SaveTasks))
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
    Api.onLoad(self.reduce((json) => Actions.TasksLoadResponse(json)));
    ReasonReact.NoUpdate
  }
};
