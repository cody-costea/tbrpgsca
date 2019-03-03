import 'package:dtbrpgsca/Actor.dart';
import 'package:dtbrpgsca/Costume.dart';
import 'package:dtbrpgsca/Costume.dart';
import 'package:dtbrpgsca/Costume.dart';
import 'package:dtbrpgsca/Performance.dart';
import 'package:dtbrpgsca/StateMask.dart';
import 'package:flutter/material.dart';
import 'package:dtbrpgsca/ArenaStage.dart';

final List<StateMask> state = [
  StateMask(1, "Regen", false, false, false, false, -1, 0, -10, 0, 0,
      0, 0, 0, 0, 2, 0, 0, 0, 0, false, null, null, null, null, null),
  StateMask(2, "Poison", false, false, false, false, 10, 0, 7, 0, 2,
      0, 0, 0, 0, -2, 0, 0, 0, 0, false, null, null, null, null, null),
  StateMask(3, "Clarity", false, false, false, false, -1, 0, 0, -7, 0,
      0, 0, 0, 0, 0, 1, 1, 0, 0, false, null, null, null, null, null),
  StateMask(4, "Dizziness", false, false, false, false, 3, 0, 0, 7, 0,
      0, 0, 0, 0, 0, -1, -1, 0, 0, false, null, null, null, null, null),
  StateMask(5, "Vigour", false, false, false, false, -1, 0, 0, 0, -7,
      0, 0, 0, 1, 0, 0, 0, 1, 0, false, null, null, null, null, null),
  StateMask(6, "Weakness", false, false, false, false, 5, 0, 0, 0, 7,
      0, 0, 0, -1, 0, 0, 0, -1, 0, false, null, null, null, null, null),
  StateMask(7, "Berserk", false, true, false, false, 7, 0, 0, 0, 0,
      0, 0, 0, 5, -3, 0, 0, 3, 0, false, null, null, null, null, null),
  StateMask(8, "Confusion", false, false, true, false, 3, 2, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, false, null, null, null, null, null),
  StateMask(9, "Sleep", true, false, false, false, 5, 1, 0, 0, 0,
      0, 0, 0, 0, -3, 0, 0, -3, 0, false, null, null, null, null, null),
  StateMask(10, "Stun", true, false, false, false, 1, 0, 0, 0, 0,
      0, 0, 0, 0, -1, 0, 0, -1, 0, false, null, null, null, null, null),
  StateMask(11, "Reflect", false, false, false, true, 7, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, false, null, null, null, null, null)
];

final List<StateMask> confsleep = [state[7], state[8]];
final List<StateMask> cure = [state[1], state[3], state[5], state[6], state[7], state[8], state[9]];
final List<StateMask> dizziness = [state[3]];
final List<StateMask> poison = [state[1]];
final List<StateMask> regen = [state[0]];
final List<StateMask> madness = [state[3], state[6], state[7], state[8], state[9]];
final List<StateMask> clarity = [state[2]];
final List<StateMask> weakness = [state[5]];
final List<StateMask> vigour = [state[4]];
final List<StateMask> tregen = [state[0], state[2], state[4]];
final List<StateMask> berserk = [state[6]];
final List<StateMask> stun = [state[9]];
final List<StateMask> reflect = [state[10]];
final List<StateMask> confusion = [state[7]];
final List<StateMask> confclarity = [state[2], state[7], state[8]];
final List<StateMask> conftregen = [state[0], state[2], state[4], state[7], state[8]];
final List<StateMask> sleep = [state[8]];
final List<StateMask> tpoison = [state[1], state[3], state[5]];
final List<StateMask> dizzystun = [state[3], state[9]];

final List<Performance> skill = [
  Performance(1, "Attack", null, null, false, false, 0, 0, 0, 0, 0, 1, 10, 0, -3, 0, 1, -1, -1, false, false, null, confsleep),
  Performance(2, "Defend", null, null, false, true, 0, 0, 0, 0, 3, 1, 0, -1, -2, -1, 0, -1, -1, false, false, null, null),
  Performance(3, "Heal", null, null, false, true, 1, 0, 3, 0, 3, 1, -25, 0, 0, 0, 6, -1, -1, false, false, null, null),
  Performance(4, "Meditate", null, null, false, true, 1, 0, 0, 0, 3, 1, -1, -7, 10, -1, 6, -1, -1, false, false, null, dizziness),
  Performance(5, "Cure", null, null, false, true, 3, 0, 7, 0, 3, 1, -17, 0, 0, 0, 7, -1, -1, false, true, null, cure),
  Performance(6, "Clariy", null, null, false, true, 3, 0, 0, 7, 3, 1, 0, -3, 0, 0, 6, -1, -1, false, false, clarity, madness),
  Performance(7, "Regen", null, null, false, true, 4, 0, 10, 0, 3, 1, -37, 0, 0, 0, 6, -1, -1, false, true, regen, poison),
  Performance(8, "Prayer", null, null, false, true, 5, 0, 7, 0, 3, 1, -23, 0, 0, 1, 0, -1, -1, false, true, null, null),
  Performance(9, "Smite", null, null, false, false, 1, 1, 0, 2, 1, 1, 4, 3, -4, 0, 1, -1, -1, false, false, null, confclarity),
  Performance(10, "Hit", null, null, false, false, 1, 3, 0, 1, 0, 1, 12, 0, -4, 0, 1, -1, -1, false, false, null, confsleep),
  Performance(11, "Bash", null, null, false, false, 3, 3, 0, 5, 1, 1, 7, 5, -5, 0, 1, -1, -1, false, false, dizziness, confclarity),
  Performance(12, "Smash", null, null, false, false, 3, 5, 0, 3, 0, 1, 18, 0, -7, 0, 1, -1, -1, false, false, null, confsleep),
  Performance(13, "Berserk", null, null, false, false, 4, 7, 0, 4, 3, 1, 0, 0, 0, -1, 0, -1, -1, false, false, berserk, weakness),
  Performance(14, "Shock", null, null, false, false, 4, 4, 0, 7, 1, 1, 10, 5, -9, 0, 7, -1, -1, false, false, dizzystun, confclarity),
  Performance(15, "Crush", null, null, false, false, 5, 7, 4, 0, 0, 1, 25, 0, -11, 0, 1, -1, -1, false, false, stun, confsleep),
  Performance(16, "Strike", null, null, false, true, 1, 0, 0, 0, 4, 1, 13, 0, -3, 0, 1, 5, 3, false, false, null, confsleep),
  Performance(17, "Steal", null, null, true, true, 1, 0, 0, 0, 3, 1, 0, 0, 0, 0, 1, 5, 3, false, false, null, null),
  Performance(18, "Dash", null, null, false, true, 3, 0, 0, 0, 4, 1, 15, 0, -5, 0, 1, 3, 7, false, false, null, confsleep),
  Performance(19, "Poison", null, null, false, true, 3, 0, 0, 0, 4, 2, 5, 0, 2, 0, 1, 3, 7, false, false, poison, regen),
  Performance(20, "Mug", null, null, true, true, 4, 0, 0, 0, 4, 2, 15, 0, -7, 0, 1, 1, 5, false, false, null, confsleep),
  Performance(21, "Toxic Gas", null, null, false, true, 4, 0, 10, 0, 6, 3, 1, 1, 1, 1, 1, 3, 7, false, false, tpoison, tregen),
  Performance(22, "Cheer", null, null, false, true, 4, 0, 10, 0, 3, -1, 0, 0, -5, -2, 0, 3, 7, false, false, vigour, cure),
  Performance(23, "Venom Blade", null, null, false, false, 5, 0, 0, 10, 4, 1, 17, 0, -9, 0, 1, 3, 7, false, false, poison, conftregen),
  Performance(24, "Absorb", null, null, false, true, 1, 0, 0, 3, 2, 1, 0, 7, -3, 0, 6, -1, -1, true, false, null, null),
  Performance(25, "Drain", null, null, false, true, 3, 0, 10, 0, 2, 1, 15, 0, -3, 0, 6, -1, -1, true, false, null, null),
  Performance(26, "Fireball", null, null, false, true, 1, 0, 3, 0, 2, 1, 11, 0, -5, 0, 2, -1, -1, false, false, null, sleep),
  Performance(27, "Iceshard", null, null, false, true, 1, 0, 3, 0, 2, 1, 11, 0, -5, 0, 3, -1, -1, false, false, null, sleep),
  Performance(28, "Lighting", null, null, false, true, 1, 0, 3, 0, 2, 1, 11, 0, -5, 0, 4, -1, -1, false, false, null, sleep),
  Performance(29, "Rock", null, null, false, true, 1, 0, 3, 0, 2, 1, 11, 0, -5, 0, 5, -1, -1, false, false, null, sleep),
  Performance(30, "Darkness", null, null, false, true, 1, 0, 3, 0, 2, 1, 11, 0, -5, 0, 6, -1, -1, false, false, null, sleep),
  Performance(31, "Flame", null, null, false, true, 3, 0, 5, 0, 2, 1, 15, 0, -7, 1, 2, -1, -1, false, false, null, sleep),
  Performance(32, "Blizzard", null, null, false, true, 3, 0, 5, 0, 2, 1, 15, 0, -7, 1, 3, -1, -1, false, false, null, sleep),
  Performance(33, "Storm", null, null, false, true, 3, 0, 5, 0, 2, 1, 15, 0, -7, 1, 4, -1, -1, false, false, null, sleep),
  Performance(34, "Earthquake", null, null, false, true, 3, 0, 5, 0, 2, 1, 15, 0, -7, 1, 5, -1, -1, false, false, null, sleep),
  Performance(35, "Eclipse", null, null, false, true, 3, 0, 5, 0, 2, 1, 13, 0, -7, 1, 6, -1, -1, false, false, null, sleep),
  Performance(36, "Flare", null, null, false, true, 5, 0, 12, 0, 2, 2, 25, 0, -9, 0, 1, -1, -1, false, false, null, sleep),
  Performance(37, "Light Ray", null, null, false, true, 1, 0, 3, 0, 3, 1, 11, 0, -5, 0, 7, -1, -1, false, false, null, sleep),
  Performance(38, "Light Burst", null, null, false, true, 3, 0, 5, 0, 3, 1, 15, 0, -7, 1, 7, -1, -1, false, false, null, sleep),
  Performance(39, "Confusion", null, null, false, true, 5, 0, 15, 0, 2, 1, 0, 0, 0, 0, 6, -1, -1, false, false, confusion, clarity),
  Performance(40, "Sleep", null, null, false, true, 4, 0, 0, 17, 4, 1, 3, 0, 17, 0, 1, -1, -1, false, false, sleep, clarity),
  Performance(41, "Slash", null, null, false, true, 5, 0, 10, 0, 4, 1, 15, 0, 0, 1, 1, 3, 7, false, false, null, confsleep),
  Performance(42, "Fire Wpn", null, null, false, false, 2, 0, 3, 2, 5, 1, 17, 0, -7, 0, 2, -1, -1, false, false, null, confsleep),
  Performance(43, "Ice Wpn", null, null, false, false, 2, 0, 3, 2, 5, 1, 17, 0, -7, 0, 3, -1, -1, false, false, null, confsleep),
  Performance(44, "Electric Wpn", null, null, false, false, 2, 0, 3, 2, 5, 1, 17, 0, -7, 0, 4, -1, -1, false, false, null, confsleep),
  Performance(45, "Stone Wpn", null, null, false, false, 2, 0, 3, 2, 5, 1, 17, 0, -7, 0, 5, -1, -1, false, false, null, confsleep),
  Performance(46, "Dark Wpn", null, null, false, false, 2, 0, 3, 2, 5, 1, 17, 0, -7, 0, 6, -1, -1, false, false, null, confsleep),
  Performance(47, "Vampiric Wpn", null, null, false, false, 5, 0, 10, 0, 5, 1, 21, 0, -9, 0, 6, 3, 7, true, false, null, confsleep),
  Performance(48, "Reflect", null, null, false, true, 5, 0, 10, 0, 3, 1, 0, 0, 0, 0, 0, -1, -1, false, false, reflect, null),
  Performance(49, "Meteor", null, null, false, true, 5, 0, 17, 0, 2, 1, 19, 0, -11, 1, 1, -1, -1, false, false, null, sleep),
  Performance(50, "Syphon", null, null, false, true, 4, 0, 15, 0, 2, 1, 13, 0, -3, 1, 6, -1, -1, true, false, null, null),
  Performance(51, "Dragon Breath", null, null, false, false, 4, 0, 13, 7, 5, 1, 15, 0, -11, 1, 1, -1, -1, false, false, null, confsleep),
  Performance(52, "Light Wpn", null, null, false, false, 2, 0, 3, 2, 7, 1, 17, 0, -5, 0, 7, -1, -1, false, false, null, confsleep),
  Performance(53, "Heal", null, null, false, true, 1, 0, 3, 0, 2, 1, -25, 0, 0, 0, 7, -1, -1, false, false, null, null),
  Performance(54, "Meditate", null, null, false, true, 1, 0, 0, 2, 2, 1, -3, -7, 0, -1, 7, -1, -1, false, false, null, dizziness),
  Performance(55, "Cure", null, null, false, true, 3, 0, 7, 0, 2, 1, -17, 0, 0, 0, 7, -1, -1, false, true, null, cure),
  Performance(56, "Clarity", null, null, false, true, 3, 0, 0, 7, 2, 1, 0, -3, 0, 0, 7, -1, -1, false, false, clarity, madness),
  Performance(57, "Absorb", null, null, false, true, 1, 0, 0, 3, 3, 1, 0, 7, -3, 0, 7, -1, -1, true, false, null, null)
];

final List<Performance> skills = [skill[0], skill[1]];

final List<Costume> race = [
  Costume(1, "Elf", null, 40, 25, 10, 7, 5, 15, 12, 11, 0, false,null, skills, null, null),
  Costume(2, "Human", null, 47, 15, 13, 9, 11, 9, 11, 10, 0, false, null, skills, null, null),
  Costume(3, "Half-Orc", null, 55, 7, 13, 17, 12, 5, 7, 9, 0, false, null, skills, null, null),
  Costume(4, "Gnome", null, 40, 15, 20, 12, 8, 10, 5, 15, 0, false, null, skills, null, null)
];

final List<Costume> job = [
  Costume(15, "Hero", "hero", 1, 1, 1, 0, 0, 0, 0, 0, 0, false, null, [skill[8], skill[9], skill[10], skill[11], skill[23], skill[24], skill[25], skill[26], skill[27], skill[28], skill[29], skill[41], skill[42], skill[43], skill[44], skill[45], skill[30], skill[31], skill[32], skill[33], skill[34], skill[36], skill[51], skill[37], skill[2], skill[3], skill[4], skill[5], skill[15], skill[16], skill[17], skill[18]], null, null),
  Costume(1, "Berserker", "berserker", 1, 0, 0, 1, 0, 0, 0, 1, 0, false, {1 : 1, 2 : -1, 3 : -1, 4 : -1, 5 : -1}, [skill[8], skill[9], skill[10], skill[11], skill[12], skill[14]], null, null),
  Costume(2, "Wizard", "wizard", 1, 0, 0, 0, 1, 0, 1, 0, 0, false, {1 : -1, 2 : 1, 3 : 1, 4 : 1, 5 : 1, 6 : -1}, [skill[23], skill[24], skill[25], skill[26], skill[27], skill[28], skill[29], skill[30], skill[31], skill[32], skill[33], skill[34], skill[35], skill[38]], null, null),
  Costume(3, "Hesychast", "hesychast", 0, 1, 0, 0, 1, 0, 1, 0, 0, false, {6 : -7, 7 : 7}, [skill[2], skill[3], skill[4], skill[5], skill[6], skill[7]], null, null),
  Costume(4, "Spy", "spy", 0, 0, 1, 1, 0, 0, 0, 1, 0, false, null, [skill[15], skill[16], skill[17], skill[18], skill[19], skill[38]], null, null),
  Costume(5, "Alchemist", "alchemist", 0, 1, 0, 0, 0, 1, 0, 1, 0, false, null, [skill[23], skill[24], skill[25], skill[26], skill[27], skill[28], skill[30], skill[31], skill[32], skill[33], skill[15], skill[16], skill[17], skill[18], skill[20]], null, null),
  Costume(6, "Dragoon", "dragoon", 1, 0, 0, 1, 0, 1, 0, 0, 0, false, {2 : 1, 3 : 1, 4 : 1, 5 : 1, 7 : -1}, [skill[8], skill[9], skill[10], skill[11], skill[23], skill[24], skill[25], skill[26], skill[27], skill[28], skill[30], skill[31], skill[32], skill[33], skill[41], skill[42], skill[43], skill[44], skill[50]], null, null),
  Costume(7, "Knight", "knight", 1, 0, 0, 0, 1, 0, 1, 0, 0, false, {1 : 1, 2 : -1, 3 : -1, 4 : -1, 5 : -1, 6 : -7, 7 : 7}, [skill[2], skill[3], skill[4], skill[5], skill[8], skill[9], skill[10], skill[11], skill[13]], null, null),
  Costume(8, "Ranger", "ranger", 0, 0, 1, 0, 0, 0, 1, 1, 0, false, {6 : -2, 7 : 2}, [skill[2], skill[3], skill[4], skill[5], skill[15], skill[16], skill[17], skill[18], skill[21]], null, null),
  Costume(9, "Shaman", "shaman", 0, 1, 0, 0, 0, 1, 1, 0, 0, false, {6 : 7, 7 : -7}, [skill[52], skill[53], skill[54], skill[55], skill[23], skill[24], skill[29], skill[34], skill[49]], null, null),
  Costume(10, "Corsair", "corsair", 0, 0, 1, 1, 0, 0, 0, 1, 0, false, null, [skill[8], skill[9], skill[10], skill[11], skill[15], skill[16], skill[17], skill[18], skill[22]], null, null),
  Costume(11, "Reaver", "reaver", 1, 0, 0, 1, 0, 0, 0, 0, 0, false, {6 : 7, 7 : -7}, [skill[8], skill[9], skill[11], skill[23], skill[24], skill[29], skill[15], skill[16], skill[18], skill[45], skill[46]], null, null),
  Costume(12, "Ninja", "ninja", 0, 0, 1, 0, 0, 0, 0, 1, 0, false, null, [skill[8], skill[9], skill[11], skill[15], skill[16], skill[17], skill[2], skill[3], skill[5], skill[40]], null, null),
  Costume(13, "Crusader", "crusader", 1, 0, 0, 0, 0, 0, 1, 0, 0, false, {6 : -7, 7 : 7}, [skill[8], skill[9], skill[10], skill[2], skill[3], skill[4], skill[56], skill[36], skill[51], skill[37], skill[47]], null, null),
  Costume(14, "Druid", "druid", 0, 1, 0, 0, 0, 1, 0, 0, 0, false, {2 : 1, 3 : 1, 4 : 1, 5 : 1, 6 : -1, 7 : -1}, [skill[23], skill[25], skill[26], skill[27], skill[28], skill[30], skill[31], skill[32], skill[33], skill[52], skill[53], skill[54], skill[15], skill[16], skill[18], skill[48]], null, null)
];

final List<Actor> party = [
  Actor(15, "Cody", race[2], job[0], 1, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, false, null, null, null, null),
  Actor(15, "Victoria", race[0], job[0], 1, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, false, null, null, null, null),
  Actor(15, "Stephanie", race[3], job[0], 1, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, false, null, null, null, null),
  Actor(15, "George", race[1], job[0], 1, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, false, null, null, null, null)
];

final List<Actor> enemy = [
  Actor(15, "Goblin", race[2], job[0], 1, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, false, null, null, null, null),
  Actor(15, "Lizard", race[0], job[0], 1, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, false, null, null, null, null),
  Actor(15, "Troll", race[3], job[0], 1, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, false, null, null, null, null),
  Actor(15, "Ogre", race[1], job[0], 1, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, false, null, null, null, null)
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
