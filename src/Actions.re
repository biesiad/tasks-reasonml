type alertAction =
  | ShowAlert(Types.alertContent)
  | CloseAlert(float);

type action =
  | AlertAction(alertAction)
  | AddTask
  | UpdateTask(float, string)
  | RemoveTask(float)
  | SaveTasks
  | TasksLoadResponse(list(Types.task))
  | TasksLoadResponseError(string)
  | TasksSaveResponse
  | TasksSaveResponseError(string);
