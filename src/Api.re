let tasksUrl = "http://cfassignment.herokuapp.com/greg/tasks";

let decodeTask = (json: Js.Json.t) : Types.task => {
  title: Json.Decode.field("title", Json.Decode.string, json),
  id: Json.Decode.field("id", Json.Decode.float, json)
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

let onLoad = (callback) =>
  Js.Promise.(
    Fetch.fetch(tasksUrl)
    |> then_(Fetch.Response.json)
    |> then_(
         (json) => {
           callback(json);
           resolve()
         }
       )
    |> ignore
  );

let onSave = (tasks, callback) =>
  Js.Promise.(
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
    |> then_(
         (response) => {
           callback(response);
           resolve()
         }
       )
    |> ignore
  );
