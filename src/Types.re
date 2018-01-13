type requestStatus =
  | Pending
  | Done;

type alertContent =
  | Success(string)
  | Error(string);

type alert = {
  id: float,
  content: alertContent
};

type task = {
  title: string,
  id: float,
  focus: bool
};
