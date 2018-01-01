type state = {inputRef: ref(option(Dom.element))};

let setInputRef = (input, {ReasonReact.state}) =>
  state.inputRef := Js.Nullable.to_opt(input);

let component = ReasonReact.reducerComponent("Task");

let make = (~onRemove, ~onChange, ~title, _children) => {
  ...component,
  initialState: () => {inputRef: ref(None)},
  reducer: ((), _state) => ReasonReact.NoUpdate,
  render: (self) =>
    <div className="task">
      <input
        className="task-input"
        ref=(self.handle(setInputRef))
        _type="text"
        defaultValue=title
        onChange
        placeholder="Task name..."
      />
      <i className="fa fa-trash-o task-delete" onClick=onRemove />
    </div>,
  didMount: (self) => {
    switch self.state.inputRef^ {
    | Some(input) => ReactDOMRe.domElementToObj(input)##focus()
    | None => ()
    };
    ReasonReact.NoUpdate
  }
};
