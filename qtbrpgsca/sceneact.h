/*
Copyright (C) AD 2018 Claudiu-Stefan Costea

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

#ifndef SCENEACT_H
#define SCENEACT_H

#include <qobject.h>
#include <actor.h>
#include <ability.h>
#include <QVector>
#include <QString>
#include <QStringList>

namespace qtbrpgsca
{

class SceneAct : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int battlerNr READ getBattlerNr)
    Q_PROPERTY(int current READ getCurrent NOTIFY currentChanged)
    Q_PROPERTY(int enemyIndex READ getEnemyIndex)
    Q_PROPERTY(int firstTarget READ getFirstTarget NOTIFY firstTargetChanged)
    Q_PROPERTY(int lastTarget READ getLastTarget NOTIFY lastTargetChanged)
    Q_PROPERTY(int status READ getStatus NOTIFY statusChanged)
    Q_PROPERTY(QStringList battlerNames READ getBattlerNames)
    Q_PROPERTY(QStringList crSkillsNames READ getCrSkillsNames NOTIFY crSkillsChanged)
    Q_PROPERTY(QStringList crItemsNames READ getCrItemsNames NOTIFY crItemsChanged)
    Q_PROPERTY(QStringList sprites READ getSprites)
public:

    SceneAct(QString arenaImage, QString arenaSong, Actor* party, int partyNr, bool copyParty, Actor* enemy, int enemyNr, bool copyEnemy, int surprise);

    Q_INVOKABLE bool setCurrentActive(bool activate);

    Q_INVOKABLE QString endTurn(QString ret);

    Q_INVOKABLE QString performSkill(int index, int target, QString ret);

    Q_INVOKABLE QString useItem(int index, int target, QString ret);

    Q_INVOKABLE QString executeAI(QString ret);

    Q_INVOKABLE int getBattlerNr();

    Q_INVOKABLE int getCurrent();

    Q_INVOKABLE int getEnemyIndex();

    Q_INVOKABLE int getFirstTarget();

    Q_INVOKABLE int getLastTarget();

    Q_INVOKABLE int getStatus();

    Q_INVOKABLE int getAIskill(int healSkill);

    Q_INVOKABLE QStringList getBattlerNames();

    Q_INVOKABLE QString getBattlerDesc(int i);

    Q_INVOKABLE QStringList getCrSkillsNames();

    Q_INVOKABLE QString getCrSkillDesc(int i);

    Q_INVOKABLE bool checkCrSkill(int i);

    Q_INVOKABLE QStringList getCrItemsNames();

    Q_INVOKABLE QString getCrItemDesc(int i);

    Q_INVOKABLE bool checkCrItem(int i);

    Q_INVOKABLE QString getArenaImageFile();

    Q_INVOKABLE QString getArenaSongFile();

    Q_INVOKABLE QString getLastAbilityAudio();

    Q_INVOKABLE QString getLastAbilityAnim();

    Q_INVOKABLE QStringList getSprites();

    Q_INVOKABLE int getGuardianVsSkill(int target, int skill);

    Q_INVOKABLE int getGuardianVsItem(int target, int item);

    Q_INVOKABLE bool checkIfKO(int actor);

    Q_INVOKABLE bool checkIfSkillHeals(int skill);

    Q_INVOKABLE bool checkIfItemHeals(int item);

    Q_INVOKABLE bool checkIfReflects(int user, int skill, int target);

    Q_INVOKABLE bool checkIfAI();

    ~SceneAct();

private:

    Actor** battler;
    QVector<Ability>* bSkills;
    QVector<Ability>* crItems;

    QString arenaImage, arenaSong;
    QString* abilityAnim,* abilityAudio;
    QStringList battlerNames, skillNames, itemNames, sprites;

    int battlerNr, status, current, enemyIndex, fTarget, lTarget;

    bool copyParty, copyEnemy;

    QString executeAbility(Ability& skill, int target, QString ret);

    int getGuardian(int target, Ability& skill);

    int checkAIheal(int ret);

    QString setAItarget(Ability& ability, QString ret);

    void setBattler(int i);

signals:
    void currentChanged();
    void crSkillsChanged();
    void crItemsChanged();
    void statusChanged();
    void firstTargetChanged();
    void lastTargetChanged();

};

}
#endif // SCENEACT_H
