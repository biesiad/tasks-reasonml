let component = ReasonReact.statelessComponent("Alert");

let renderAlert = (className, message, id, onHide) =>
  <div className=("alert alert-" ++ className) key=(string_of_float(id))>
    (ReasonReact.stringToElement(message))
    <i className="fa fa-times alert-close" onClick=onHide />
  </div>;

let make = (~alert: Types.alert, ~onHide, _children) => {
  ...component,
  render: (_self) =>
    switch alert.content {
    | Types.Success(message) =>
      renderAlert("success", message, alert.id, onHide)
    | Types.Error(message) => renderAlert("error", message, alert.id, onHide)
    }
};
