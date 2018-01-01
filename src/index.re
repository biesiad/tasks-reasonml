[%bs.raw {| require('./index.html') |}];

[%bs.raw {| require('./index.scss') |}];

ReactDOMRe.renderToElementWithId(<Page message="Hello!" />, "app");
