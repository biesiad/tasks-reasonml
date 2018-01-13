type alertAction =
  | ShowAlert(Types.alertContent)
  | CloseAlert(float);

type action =
  | AlertAction(alertAction)
  | AddTask
  | UpdateTask(float, string)
  | RemoveTask(float)
  | SaveTasks
  | TasksLoadResponse(Js.Json.t)
  | TasksSaveResponse(Fetch.Response.t);
