/*
Copyright (C) AD 2019 Claudiu-Stefan Costea

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/
import 'package:dtbrpgsca/Actor.dart';
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
  Performance(1, "Attack", null, null, false, false, 0, 0, 0, 0, Performance.DmgTypeAtk, 1, 10, 0, -3, 0, 1, -1, -1, false, false, null, confsleep),
  Performance(2, "Defend", null, null, false, true, 0, 0, 0, 0, Performance.DmgTypeSpi, 1, 0, -1, -2, -1, 0, -1, -1, false, false, null, null),
  Performance(3, "Heal", null, null, false, true, 1, 0, 3, 0, Performance.DmgTypeSpi, 1, -25, 0, 0, 0, 6, -1, -1, false, false, null, null),
  Performance(4, "Meditate", null, null, false, true, 1, 0, 0, 0, Performance.DmgTypeSpi, 1, -1, -7, 10, -1, 6, -1, -1, false, false, null, dizziness),
  Performance(5, "Cure", null, null, false, true, 3, 0, 7, 0, Performance.DmgTypeSpi, 1, -17, 0, 0, 0, 7, -1, -1, false, true, null, cure),
  Performance(6, "Clariy", null, null, false, true, 3, 0, 0, 7, Performance.DmgTypeSpi, 1, 0, -3, 0, 0, 6, -1, -1, false, false, clarity, madness),
  Performance(7, "Regen", null, null, false, true, 4, 0, 10, 0, Performance.DmgTypeSpi, 1, -37, 0, 0, 0, 6, -1, -1, false, true, regen, poison),
  Performance(8, "Prayer", null, null, false, true, 5, 0, 7, 0, Performance.DmgTypeSpi, 1, -23, 0, 0, 1, 0, -1, -1, false, true, null, null),
  Performance(9, "Smite", null, null, false, false, 1, 1, 0, 2, Performance.DmgTypeAtk | Performance.DmgTypeDef, 1, 4, 3, -4, 0, 1, -1, -1, false, false, null, confclarity),
  Performance(10, "Hit", null, null, false, false, 1, 3, 0, 1, Performance.DmgTypeAtk, 1, 12, 0, -4, 0, 1, -1, -1, false, false, null, confsleep),
  Performance(11, "Bash", null, null, false, false, 3, 3, 0, 5, Performance.DmgTypeAtk | Performance.DmgTypeDef, 1, 7, 5, -5, 0, 1, -1, -1, false, false, dizziness, confclarity),
  Performance(12, "Smash", null, null, false, false, 3, 5, 0, 3, Performance.DmgTypeAtk, 1, 18, 0, -7, 0, 1, -1, -1, false, false, null, confsleep),
  Performance(13, "Berserk", null, null, false, false, 4, 7, 0, 4, Performance.DmgTypeSpi, 1, 0, 0, 0, -1, 0, -1, -1, false, false, berserk, weakness),
  Performance(14, "Shock", null, null, false, false, 4, 4, 0, 7, Performance.DmgTypeAtk | Performance.DmgTypeDef, 1, 10, 5, -9, 0, 7, -1, -1, false, false, dizzystun, confclarity),
  Performance(15, "Crush", null, null, false, false, 5, 7, 4, 0, Performance.DmgTypeAtk, 1, 25, 0, -11, 0, 1, -1, -1, false, false, stun, confsleep),
  Performance(16, "Strike", null, null, false, true, 1, 0, 0, 0, Performance.DmgTypeAtk | Performance.DmgTypeAgi, 1, 13, 0, -3, 0, 1, 5, 3, false, false, null, confsleep),
  Performance(17, "Steal", null, null, true, true, 1, 0, 0, 0, Performance.DmgTypeWis, 1, 0, 0, 0, 0, 1, 5, 3, false, false, null, null),
  Performance(18, "Dash", null, null, false, true, 3, 0, 0, 0, Performance.DmgTypeAtk | Performance.DmgTypeAgi, 1, 15, 0, -5, 0, 1, 3, 7, false, false, null, confsleep),
  Performance(19, "Poison", null, null, false, true, 3, 0, 0, 0, Performance.DmgTypeWis, 2, 5, 0, 2, 0, 1, 3, 7, false, false, poison, regen),
  Performance(20, "Mug", null, null, true, true, 4, 0, 0, 0, Performance.DmgTypeAtk | Performance.DmgTypeAgi, 2, 15, 0, -7, 0, 1, 1, 5, false, false, null, confsleep),
  Performance(21, "Toxic Gas", null, null, false, true, 4, 0, 10, 0, Performance.DmgTypeWis, 3, 1, 1, 1, 1, 1, 3, 7, false, false, tpoison, tregen),
  Performance(22, "Cheer", null, null, false, true, 4, 0, 10, 0, Performance.DmgTypeSpi, -1, 0, 0, -5, -2, 0, 3, 7, false, false, vigour, cure),
  Performance(23, "Venom Blade", null, null, false, false, 5, 0, 0, 10, Performance.DmgTypeAtk | Performance.DmgTypeAgi, 1, 17, 0, -9, 0, 1, 3, 7, false, false, poison, conftregen),
  Performance(24, "Absorb", null, null, false, true, 1, 0, 0, 3, Performance.DmgTypeWis, 1, 0, 7, -3, 0, 6, -1, -1, true, false, null, null),
  Performance(25, "Drain", null, null, false, true, 3, 0, 10, 0, Performance.DmgTypeWis, 1, 15, 0, -3, 0, 6, -1, -1, true, false, null, null),
  Performance(26, "Fireball", null, null, false, true, 1, 0, 3, 0, Performance.DmgTypeWis, 1, 11, 0, -5, 0, 2, -1, -1, false, false, null, sleep),
  Performance(27, "Iceshard", null, null, false, true, 1, 0, 3, 0, Performance.DmgTypeWis, 1, 11, 0, -5, 0, 3, -1, -1, false, false, null, sleep),
  Performance(28, "Lighting", null, null, false, true, 1, 0, 3, 0, Performance.DmgTypeWis, 1, 11, 0, -5, 0, 4, -1, -1, false, false, null, sleep),
  Performance(29, "Rock", null, null, false, true, 1, 0, 3, 0, Performance.DmgTypeWis, 1, 11, 0, -5, 0, 5, -1, -1, false, false, null, sleep),
  Performance(30, "Darkness", null, null, false, true, 1, 0, 3, 0, Performance.DmgTypeWis, 1, 11, 0, -5, 0, 6, -1, -1, false, false, null, sleep),
  Performance(31, "Flame", null, null, false, true, 3, 0, 5, 0, Performance.DmgTypeWis, 1, 15, 0, -7, 1, 2, -1, -1, false, false, null, sleep),
  Performance(32, "Blizzard", null, null, false, true, 3, 0, 5, 0, Performance.DmgTypeWis, 1, 15, 0, -7, 1, 3, -1, -1, false, false, null, sleep),
  Performance(33, "Storm", null, null, false, true, 3, 0, 5, 0, Performance.DmgTypeWis, 1, 15, 0, -7, 1, 4, -1, -1, false, false, null, sleep),
  Performance(34, "Earthquake", null, null, false, true, 3, 0, 5, 0, Performance.DmgTypeWis, 1, 15, 0, -7, 1, 5, -1, -1, false, false, null, sleep),
  Performance(35, "Eclipse", null, null, false, true, 3, 0, 5, 0, Performance.DmgTypeWis, 1, 13, 0, -7, 1, 6, -1, -1, false, false, null, sleep),
  Performance(36, "Flare", null, null, false, true, 5, 0, 12, 0, Performance.DmgTypeWis, 2, 25, 0, -9, 0, 1, -1, -1, false, false, null, sleep),
  Performance(37, "Light Ray", null, null, false, true, 1, 0, 3, 0, Performance.DmgTypeSpi, 1, 11, 0, -5, 0, 7, -1, -1, false, false, null, sleep),
  Performance(38, "Light Burst", null, null, false, true, 3, 0, 5, 0, Performance.DmgTypeSpi, 1, 15, 0, -7, 1, 7, -1, -1, false, false, null, sleep),
  Performance(39, "Confusion", null, null, false, true, 5, 0, 15, 0, Performance.DmgTypeWis, 1, 0, 0, 0, 0, 6, -1, -1, false, false, confusion, clarity),
  Performance(40, "Sleep", null, null, false, true, 4, 0, 0, 17, Performance.DmgTypeWis, 1, 3, 0, 17, 0, 1, -1, -1, false, false, sleep, clarity),
  Performance(41, "Slash", null, null, false, true, 5, 0, 10, 0, Performance.DmgTypeAtk | Performance.DmgTypeAgi, 1, 15, 0, 0, 1, 1, 3, 7, false, false, null, confsleep),
  Performance(42, "Fire Wpn", null, null, false, false, 2, 0, 3, 2, Performance.DmgTypeAtk | Performance.DmgTypeWis, 1, 17, 0, -7, 0, 2, -1, -1, false, false, null, confsleep),
  Performance(43, "Ice Wpn", null, null, false, false, 2, 0, 3, 2, Performance.DmgTypeAtk | Performance.DmgTypeWis, 1, 17, 0, -7, 0, 3, -1, -1, false, false, null, confsleep),
  Performance(44, "Electric Wpn", null, null, false, false, 2, 0, 3, 2, Performance.DmgTypeAtk | Performance.DmgTypeWis, 1, 17, 0, -7, 0, 4, -1, -1, false, false, null, confsleep),
  Performance(45, "Stone Wpn", null, null, false, false, 2, 0, 3, 2, Performance.DmgTypeAtk | Performance.DmgTypeWis, 1, 17, 0, -7, 0, 5, -1, -1, false, false, null, confsleep),
  Performance(46, "Dark Wpn", null, null, false, false, 2, 0, 3, 2, Performance.DmgTypeAtk | Performance.DmgTypeWis, 1, 17, 0, -7, 0, 6, -1, -1, false, false, null, confsleep),
  Performance(47, "Vampiric Wpn", null, null, false, false, 5, 0, 10, 0, Performance.DmgTypeAtk | Performance.DmgTypeAgi | Performance.DmgTypeWis,
      1, 21, 0, -9, 0, 6, 3, 7, true, false, null, confsleep),
  Performance(48, "Reflect", null, null, false, true, 5, 0, 10, 0, Performance.DmgTypeSpi, 1, 0, 0, 0, 0, 0, -1, -1, false, false, reflect, null),
  Performance(49, "Meteor", null, null, false, true, 5, 0, 17, 0, Performance.DmgTypeWis, 1, 19, 0, -11, 1, 1, -1, -1, false, false, null, sleep),
  Performance(50, "Syphon", null, null, false, true, 4, 0, 15, 0, Performance.DmgTypeWis, 1, 13, 0, -3, 1, 6, -1, -1, true, false, null, null),
  Performance(51, "Dragon Breath", null, null, false, false, 4, 0, 13, 7, Performance.DmgTypeAtk | Performance.DmgTypeWis, 1, 15, 0, -11, 1, 1, -1, -1, false, false, null, confsleep),
  Performance(52, "Light Wpn", null, null, false, false, 2, 0, 3, 2, Performance.DmgTypeAtk | Performance.DmgTypeDef | Performance.DmgTypeSpi,
      1, 17, 0, -5, 0, 7, -1, -1, false, false, null, confsleep),
  Performance(53, "Heal", null, null, false, true, 1, 0, 3, 0, Performance.DmgTypeWis, 1, -25, 0, 0, 0, 7, -1, -1, false, false, null, null),
  Performance(54, "Meditate", null, null, false, true, 1, 0, 0, Performance.DmgTypeWis, 2, 1, -3, -7, 0, -1, 7, -1, -1, false, false, null, dizziness),
  Performance(55, "Cure", null, null, false, true, 3, 0, 7, 0, Performance.DmgTypeWis, 1, -17, 0, 0, 0, 7, -1, -1, false, true, null, cure),
  Performance(56, "Clarity", null, null, false, true, 3, 0, 0, 7, Performance.DmgTypeWis, 1, 0, -3, 0, 0, 7, -1, -1, false, false, clarity, madness),
  Performance(57, "Absorb", null, null, false, true, 1, 0, 0, 3, Performance.DmgTypeSpi, 1, 0, 7, -3, 0, 7, -1, -1, true, false, null, null)
];

final List<Performance> skills = [skill[0], skill[1]];

final List<Costume> race = [
  Costume(1, "Elf", null, 40, 25, 10, 7, 5, 15, 12, 11, 0, false, {1 : -1, 2 : 1, 3 : 1, 4 : 1, 5 : 1}, skills, null, null),
  Costume(2, "Human", null, 47, 15, 13, 9, 11, 9, 11, 10, 0, false, {6 : -1, 7 : 1}, skills, null, null),
  Costume(3, "Half-Orc", null, 55, 7, 13, 17, 12, 5, 7, 9, 0, false, {1 : 1, 2 : -1, 3 : -1, 4 : -1, 5 : -1}, skills, null, null),
  Costume(4, "Gnome", null, 40, 15, 20, 12, 8, 10, 5, 15, 0, false, {6 : 1, 7 : -1}, skills, null, null)
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
  Costume(14, "Druid", "druid", 0, 1, 0, 0, 0, 1, 0, 0, 0, false, {2 : 1, 3 : 1, 4 : 1, 5 : 1, 6 : -1, 7 : -1}, [skill[23], skill[25], skill[26], skill[27], skill[28], skill[30], skill[31], skill[32], skill[33], skill[52], skill[53], skill[54], skill[15], skill[16], skill[18], skill[48]], null, null),
  Costume(16, "Sorceress", "sorceress", 1, 0, 0, 0, 1, 0, 1, 0, 0, false, {1 : -1, 2 : 1, 3 : 1, 4 : 1, 5 : 1, 6 : -1}, [skill[23], skill[24], skill[25], skill[26], skill[27], skill[28], skill[29], skill[30], skill[31], skill[32], skill[33], skill[34], skill[35], skill[38]], null, null),
  Costume(17, "Valkyrie", "valkyrie", 1, 0, 0, 1, 0, 0, 0, 1, 0, false, {1 : 1, 2 : -1, 3 : -1, 4 : -1, 5 : -1}, [skill[8], skill[9], skill[10], skill[11], skill[12], skill[14]], null, null)
];

final List<Actor> party = [
  Actor(1, "Cody", race[2], job[0], 1, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, false, null, null, null, null),
  Actor(3, "Victoria", race[0], job[16], 1, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, false, null, null, null, null),
  Actor(5, "Stephanie", race[3], job[15], 1, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, false, null, null, null, null),
  Actor(7, "George", race[1], job[13], 1, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, false, null, null, null, null)
];

final List<Actor> enemy = [
  Actor(2, "Goblin", race[2], job[12], 1, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, false, null, null, null, null),
  Actor(4, "Lizard", race[0], job[6], 1, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, false, null, null, null, null),
  Actor(6, "Troll", race[3], job[9], 1, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, false, null, null, null, null),
  Actor(8, "Ogre", race[1], job[1], 1, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, false, null, null, null, null)
];

void main() => runApp(new ArenaStage(party: party, enemy: enemy, surprise: 0));