type action =
  | AddTask
  | UpdateTask(float, string)
  | RemoveTask(float)
  | SaveTasks
  | TasksLoadResponse(Js.Json.t)
  | TasksSaveResponse(Fetch.Response.t)
  | ShowAlert(Types.alertContent)
  | CloseAlert(float);
