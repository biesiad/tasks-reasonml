let tasksUrl = "http://cfassignment.herokuapp.com/greg/tasks";

let decodeTask = (json: Js.Json.t) : Types.task => {
  title: Json.Decode.field("title", Json.Decode.string, json),
  id: Json.Decode.field("id", Json.Decode.float, json),
  focus: false
};

let decodeTasks = (json: Js.Json.t) : option(list(Types.task)) =>
  Json.Decode.field(
    "tasks",
    Json.Decode.optional(Json.Decode.list(decodeTask)),
    json
  );

let encodeTask = (task: Types.task) : Js.Json.t =>
  Json.Encode.object_([
    ("title", Json.Encode.string(task.title)),
    ("id", Json.Encode.float(task.id))
  ]);

let encodeTasks = (taskList: list(Types.task)) : Js.Json.t => {
  let stuff = Js.Array.map(encodeTask, Array.of_list(taskList));
  Json.Encode.object_([("tasks", Json.Encode.jsonArray(stuff))])
};

let loadTasks = (send) =>
  Fetch.fetch(tasksUrl)
  |> Js.Promise.then_(Fetch.Response.json)
  |> Js.Promise.then_(
       (json) =>
         switch (decodeTasks(json)) {
         | Some(tasks) =>
           Js.Promise.resolve(send(Actions.TasksLoadResponse(tasks)))
         | None =>
           Js.Promise.resolve(
             send(
               Actions.TasksLoadResponseError(
                 "Can't load tasks (Parsing error)."
               )
             )
           )
         }
     )
  |> Js.Promise.catch(
       (_) =>
         Js.Promise.resolve(
           send(
             Actions.TasksLoadResponseError(
               "Can't load tasks (Network error)."
             )
           )
         )
     )
  |> ignore;

let saveTasks = (send, tasks) =>
  Fetch.fetchWithInit(
    tasksUrl,
    Fetch.RequestInit.make(
      ~method_=Post,
      ~headers=
        Fetch.HeadersInit.makeWithArray([|
          ("Content-Type", "application/json; charset=utf-8")
        |]),
      ~body=Fetch.BodyInit.make(Js.Json.stringify(encodeTasks(tasks))),
      ()
    )
  )
  |> Js.Promise.then_(
       (response) =>
         if (Fetch.Response.ok(response)) {
           Js.Promise.resolve(send(Actions.TasksSaveResponse))
         } else {
           Js.Promise.resolve(
             send(
               Actions.TasksSaveResponseError(
                 "Can't save tasks (Bad request)."
               )
             )
           )
         }
     )
  |> Js.Promise.catch(
       (_) =>
         Js.Promise.resolve(
           send(
             Actions.TasksSaveResponseError(
               "Can't save tasks (Network error)."
             )
           )
         )
     )
  |> ignore;
