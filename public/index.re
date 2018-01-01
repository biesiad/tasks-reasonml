[%bs.raw {| require('../../../public/index.html') |}];

[%bs.raw {| require('font-awesome/css/font-awesome.css') |}];

[%bs.raw {| require('../../../public/index.scss') |}];

ReactDOMRe.renderToElementWithId(<App />, "app");
