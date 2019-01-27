import 'package:dtbrpgsca/Actor.dart';
import 'package:dtbrpgsca/Costume.dart';
import 'package:dtbrpgsca/Performance.dart';
import 'package:flutter/material.dart';
import 'package:dtbrpgsca/ArenaStage.dart';

final List<Performance> skills = [
  Performance(1, "Attack", false, false, 0, 0, 0, 0, 0, 1, 10, 0, -3, 0, 1, -1, -1, false, false, null, null),
  Performance(2, "Defend", false, true, 0, 0, 0, 0, 3, 1, 0, -1, -2, -1, 0, -1, -1, false, false, null, null),
  Performance(3, "Heal", false, true, 1, 0, 3, 0, 3, 1, -25, 0, 0, 0, 6, -1, -1, false, true, null, null)
];

final Costume heroJob = Costume(15, "Hero", 1, 1, 1, 0, 0, 0, 0, 0, 0, false, null, skills, null, null);
final Costume humanRace = Costume(2, "Human", 47, 15, 13, 9, 11, 9, 11, 10, 0, false, null, null, null, null);

final List<Actor> party = [
  Actor(15, "Cody", humanRace, heroJob, 1, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, false, null, null, null, null),
  Actor(15, "Victoria", humanRace, heroJob, 1, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, false, null, null, null, null),
  Actor(15, "Stephanie", humanRace, heroJob, 1, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, false, null, null, null, null),
  Actor(15, "George", humanRace, heroJob, 1, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, false, null, null, null, null)
];

final List<Actor> enemy = [
  Actor(15, "Goblin", humanRace, heroJob, 1, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, false, null, null, null, null),
  Actor(15, "Lizard", humanRace, heroJob, 1, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, false, null, null, null, null),
  Actor(15, "Troll", humanRace, heroJob, 1, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, false, null, null, null, null),
  Actor(15, "Ogre", humanRace, heroJob, 1, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, false, null, null, null, null)
];

void main() => runApp(new ArenaStage(party: party, enemy: enemy, surprise: 0));

class MyApp extends StatelessWidget {
  // This widget is the root of your application.
  @override
  Widget build(BuildContext context) {
    return new MaterialApp(
      title: 'Flutter Demo',
      theme: new ThemeData(
        // This is the theme of your application.
        //
        // Try running your application with "flutter run". You'll see the
        // application has a blue toolbar. Then, without quitting the app, try
        // changing the primarySwatch below to Colors.green and then invoke
        // "hot reload" (press "r" in the console where you ran "flutter run",
        // or press Run > Flutter Hot Reload in IntelliJ). Notice that the
        // counter didn't reset back to zero; the application is not restarted.
        primarySwatch: Colors.blue,
      ),
      home: new MyHomePage(title: 'Flutter Demo Home Page'),
    );
  }
}

class MyHomePage extends StatefulWidget {
  MyHomePage({Key key, this.title}) : super(key: key);

  // This widget is the home page of your application. It is stateful, meaning
  // that it has a State object (defined below) that contains fields that affect
  // how it looks.

  // This class is the configuration for the state. It holds the values (in this
  // case the title) provided by the parent (in this case the App widget) and
  // used by the build method of the State. Fields in a Widget subclass are
  // always marked "final".

  final String title;

  @override
  _MyHomePageState createState() => new _MyHomePageState();
}

class _MyHomePageState extends State<MyHomePage> {
  int _counter = 0;

  void _incrementCounter() {
    setState(() {
      // This call to setState tells the Flutter framework that something has
      // changed in this State, which causes it to rerun the build method below
      // so that the display can reflect the updated values. If we changed
      // _counter without calling setState(), then the build method would not be
      // called again, and so nothing would appear to happen.
      _counter++;
    });
  }

  @override
  Widget build(BuildContext context) {
    // This method is rerun every time setState is called, for instance as done
    // by the _incrementCounter method above.
    //
    // The Flutter framework has been optimized to make rerunning build methods
    // fast, so that you can just rebuild anything that needs updating rather
    // than having to individually change instances of widgets.
    return new Scaffold(
      appBar: new AppBar(
        // Here we take the value from the MyHomePage object that was created by
        // the App.build method, and use it to set our appbar title.
        title: new Text(widget.title),
      ),
      body: new Center(
        // Center is a layout widget. It takes a single child and positions it
        // in the middle of the parent.
        child: new Column(
          // Column is also layout widget. It takes a list of children and
          // arranges them vertically. By default, it sizes itself to fit its
          // children horizontally, and tries to be as tall as its parent.
          //
          // Invoke "debug paint" (press "p" in the console where you ran
          // "flutter run", or select "Toggle Debug Paint" from the Flutter tool
          // window in IntelliJ) to see the wireframe for each widget.
          //
          // Column has various properties to control how it sizes itself and
          // how it positions its children. Here we use mainAxisAlignment to
          // center the children vertically; the main axis here is the vertical
          // axis because Columns are vertical (the cross axis would be
          // horizontal).
          mainAxisAlignment: MainAxisAlignment.center,
          children: <Widget>[
            new Text(
              'You have pushed the button this many times:',
            ),
            new Text(
              '$_counter',
              style: Theme.of(context).textTheme.display1,
            ),
          ],
        ),
      ),
      floatingActionButton: new FloatingActionButton(
        onPressed: _incrementCounter,
        tooltip: 'Increment',
        child: new Icon(Icons.add),
      ), // This trailing comma makes auto-formatting nicer for build methods.
    );
  }
}
