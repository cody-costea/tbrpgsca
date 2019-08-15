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
package com.codycostea.tbrpgsca.demo

import android.content.Context
import android.graphics.Color
import android.graphics.drawable.AnimationDrawable
import android.graphics.drawable.Drawable
import android.media.MediaPlayer
import android.support.v7.app.AppCompatActivity
import android.os.Bundle
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.*
import java.util.*
import kotlin.collections.LinkedHashMap
import android.graphics.Bitmap
import android.graphics.drawable.BitmapDrawable
import android.text.method.ScrollingMovementMethod
import android.content.res.Resources
import android.os.Binder
import android.support.annotation.DrawableRes
import android.support.annotation.IdRes
import android.support.annotation.RawRes
import android.support.v4.app.Fragment
import android.support.v7.app.AlertDialog
import android.util.Log
import com.codycostea.tbrpgsca.R
import com.codycostea.tbrpgsca.library.*
import org.mozilla.javascript.Scriptable

class AdCostume(id : Int, name : String, var sprName : String, mHp : Int = 30, mMp : Int = 10, mSp : Int = 10, atk : Int = 7, def: Int = 7,
                spi: Int = 7, wis : Int = 7, agi : Int = 7, mActions: Int = 1, range: Boolean = false, res : MutableMap<Int, Int>? = null,
                skills : Array<Ability>? = null, states : Array<State>? = null, stRes : MutableMap<State, Int>? = null, mInit: Int = 0)
    : Costume(id, name, mHp, mMp, mSp, atk, def, spi, wis, agi, mActions, mInit, range, res, skills, states, stRes) {

    override fun equals(other: Any?): Boolean {
        return super.equals(other) || (other is AdCostume && other.id == this.id)
    }

    override fun hashCode(): Int {
        return javaClass.hashCode()
    }

}

class AdActor(id : Int, private val context : Context, name: String, sprites : Array<Array<AnimationDrawable>>? = null, race: Costume,
              job: AdCostume, level : Int = 1, maxLv: Int = 9, mActions : Int = 1, mHp: Int = 15, mMp: Int = 7, mSp: Int = 7, mAtk: Int = 5,
              mDef: Int = 5, mSpi: Int = 5, mWis: Int = 5, mAgi: Int = 5, range : Boolean = false, mRes: MutableMap<Int, Int>? = null,
              skills: Array<Ability>? = null, states: Array<State>? = null, mStRes: MutableMap<State, Int>? = null, mInit: Int = 0)
    : Actor(id, name, race, job, level, maxLv, mActions, mInit, mHp, mMp, mSp, mAtk, mDef, mSpi, mWis, mAgi, range, mRes, skills, states, mStRes) {

    override var job : Costume = job
        set(value) {
            super.job = value
            this.sprites = arrayOf(arrayOfNulls(7), arrayOfNulls(7))
            this.spritesDur = arrayOf(intArrayOf(0, 0, 0, 0, 0, 0, 0), intArrayOf(0, 0, 0, 0, 0, 0, 0))
            field = value
        }

    internal var spritesDur = arrayOf(intArrayOf(0, 0, 0, 0, 0, 0, 0), intArrayOf(0, 0, 0, 0, 0, 0, 0))
    private var sprites : Array<Array<AnimationDrawable?>> = arrayOf(arrayOfNulls(7), arrayOfNulls(7))
    //private var sprites : Array<Array<AnimationDrawable?>> = this.setAllBtSprites()

    fun getBtSprite(side : Int, spr : Int) : AnimationDrawable? {
        var sprAnim = this.sprites[side][spr]
        if (sprAnim === null) {
            val job = this.job
            var sprName = "spr_bt_" + ((job as? AdCostume)?.sprName?.toLowerCase() ?: job.name.toLowerCase()) + if (side == 0) "_l_" else "_r_"
            sprName += when (spr) {
                0 -> "idle"
                1 -> "ko"
                2 -> "hit"
                3 -> "fallen"
                4 -> "restored"
                5 -> "act"
                6 -> "cast"
                else -> return null
            }
            try {
                val drawable = this.context.resources.getDrawable(
                        this.context.resources.getIdentifier(sprName, "drawable", this.context.packageName))
                when (drawable) {
                    is AnimationDrawable -> sprAnim = drawable
                    is BitmapDrawable -> {
                        sprAnim = (drawable.getSprite(this.context,
                                if (spr in 0..1) null
                                else this.getBtSprite(side, 0)?.getFrame(0), spr in 2..3,
                                if (spr < 2) null else (if (spr == 3) this.getBtSprite(side, 1)?.getFrame(0)
                                else this.getBtSprite(side, 0)?.getFrame(0)), true))
                    }
                }
            }
            catch (e : Resources.NotFoundException) {
                sprAnim = when (spr) {
                    4 -> this.getBtSprite(side, 3)?.getInvertedSprite(true)
                    6 -> this.getBtSprite(side, 5)
                    else -> null
                }
            }
            finally {
                this.sprites[side][spr] = sprAnim
                this.spritesDur[side][spr] = sprAnim?.fullDur ?: 0
            }
        }
        return sprAnim
    }

    private fun setAllBtSprites(spriteName: String? = null) : Array<Array<AnimationDrawable?>> {
        val sprName : String
        sprName = if (spriteName === null) {
            val job = this.job
            (job as? AdCostume)?.sprName?.toLowerCase() ?: job.name.toLowerCase()
        }
        else {
            spriteName.toLowerCase()
        }
        val sprites = arrayOf(
                arrayOf(this.context.resources.getDrawable(
                                this.context.resources.getIdentifier("spr_bt_" + sprName + "_l_idle", "drawable", this.context.packageName))
                                as AnimationDrawable?,
                        this.context.resources.getDrawable(
                                this.context.resources.getIdentifier("spr_bt_" + sprName + "_l_ko", "drawable", this.context.packageName))
                                as AnimationDrawable?,
                        this.context.resources.getDrawable(
                                this.context.resources.getIdentifier("spr_bt_" + sprName + "_l_hit", "drawable", this.context.packageName))
                                as AnimationDrawable?,
                        this.context.resources.getDrawable(
                                this.context.resources.getIdentifier("spr_bt_" + sprName + "_l_fallen", "drawable", this.context.packageName))
                                as AnimationDrawable?,
                        this.context.resources.getDrawable(
                                this.context.resources.getIdentifier("spr_bt_" + sprName + "_l_restored", "drawable", this.context.packageName))
                                as AnimationDrawable?,
                        this.context.resources.getDrawable(
                                this.context.resources.getIdentifier("spr_bt_" + sprName + "_l_act", "drawable", this.context.packageName))
                                as AnimationDrawable?,
                        this.context.resources.getDrawable(
                                this.context.resources.getIdentifier("spr_bt_" + sprName + "_l_cast", "drawable", this.context.packageName))
                                as AnimationDrawable?),
                arrayOf(this.context.resources.getDrawable(
                                this.context.resources.getIdentifier("spr_bt_" + sprName + "_r_idle", "drawable", this.context.packageName))
                                as AnimationDrawable?,
                        this.context.resources.getDrawable(
                                this.context.resources.getIdentifier("spr_bt_" + sprName + "_r_ko", "drawable", this.context.packageName))
                                as AnimationDrawable?,
                        this.context.resources.getDrawable(
                                this.context.resources.getIdentifier("spr_bt_" + sprName + "_r_hit", "drawable", this.context.packageName))
                                as AnimationDrawable?,
                        this.context.resources.getDrawable(
                                this.context.resources.getIdentifier("spr_bt_" + sprName + "_r_fallen", "drawable", this.context.packageName))
                                as AnimationDrawable?,
                        this.context.resources.getDrawable(
                                this.context.resources.getIdentifier("spr_bt_" + sprName + "_r_restored", "drawable", this.context.packageName))
                                as AnimationDrawable?,
                        this.context.resources.getDrawable(
                                this.context.resources.getIdentifier("spr_bt_" + sprName + "_r_act", "drawable", this.context.packageName))
                                as AnimationDrawable?,
                        this.context.resources.getDrawable(
                                this.context.resources.getIdentifier("spr_bt_" + sprName + "_r_cast", "drawable", this.context.packageName))
                                as AnimationDrawable?)
        )
        this.sprites = sprites
        this.spritesDur = arrayOf(
                intArrayOf(sprites[0][0]?.fullDur ?: 0, sprites[0][1]?.fullDur ?: 0, sprites[0][2]?.fullDur ?: 0, sprites[0][3]?.fullDur ?: 0,
                        sprites[0][4]?.fullDur ?: 0, sprites[0][4]?.fullDur ?: 0, sprites[0][5]?.fullDur ?: 0, sprites[0][6]?.fullDur ?: 0),
                intArrayOf(sprites[1][0]?.fullDur ?: 0, sprites[1][1]?.fullDur ?: 0, sprites[1][2]?.fullDur ?: 0, sprites[1][3]?.fullDur ?: 0,
                        sprites[1][4]?.fullDur ?: 0, sprites[1][4]?.fullDur ?: 0, sprites[1][5]?.fullDur ?: 0, sprites[1][6]?.fullDur ?: 0)
        )
        return sprites
    }

    override fun equals(other: Any?): Boolean {
        return super.equals(other) || (other is AdActor && other.id == this.id)
    }

    override fun hashCode(): Int {
        return javaClass.hashCode()
    }

}

class AdAbility(id: Int, name: String, private val sprId : Int, private val sndId : Int, steal: Boolean = false,
                range: Boolean? = null, lvRq: Int, hpC: Int, mpC: Int, spC: Int, dmgType: Int, atkI: Int, hpDmg: Int,
                mpDmg: Int, spDmg: Int, trg : Int, elm: Int, mQty: Int, rQty: Int, absorb: Boolean, restoreKO: Boolean,
                aStates: Array<State>? = null, rStates: Array<State>? = null)
    : Ability(id, name, range, steal, lvRq, hpC, mpC, spC, hpDmg, mpDmg, spDmg, dmgType, atkI, trg, elm, mQty,
              rQty, absorb, restoreKO, aStates, rStates) {

    private var _context : Context? = null
    private var _sndPlayer : MediaPlayer? = null
    private var _sprAnim : AnimationDrawable? = null
    var spriteDur : Int = 0

    fun getSprite(context : Context) : AnimationDrawable? {
        if (this.sprId < 1) return null
        var sprAnim = this._sprAnim
        if (this._context !== context) {
            sprAnim = null
            val sndPlayer = this._sndPlayer
            if (sndPlayer !== null) {
                sndPlayer.release()
                this._sndPlayer = null
            }
        }
        if (sprAnim === null) {
            val drawable = context.resources.getDrawable(this.sprId)
            when (drawable) {
                is AnimationDrawable -> sprAnim = drawable
                is BitmapDrawable -> sprAnim = drawable.getSprite(context, null,
                        false, null, false)
            }
            if (sprAnim === null) {
                this.spriteDur = 0
            }
            else {
                this.spriteDur = sprAnim.fullDur
            }
            this._sprAnim = sprAnim
        }
        return sprAnim
    }

    fun playSound(context: Context): Int {
        if (this.sndId < 1) return 0
        var sndPlayer = this._sndPlayer
        if (this._context !== context) {
            this._sprAnim = null
            this.spriteDur = 0
            if (sndPlayer !== null) {
                sndPlayer.release()
                sndPlayer = null
            }
        }
        if (sndPlayer === null) {
            sndPlayer = MediaPlayer.create(context, this.sndId)
            if (sndPlayer === null) return 0
            this._context = context
            sndPlayer.isLooping = false
            this._sndPlayer = sndPlayer
        }
        sndPlayer.start()
        return sndPlayer.duration
    }

    override fun equals(other: Any?): Boolean {
        return super.equals(other) || (other is AdAbility && other.id == this.id)
    }

    override fun hashCode(): Int {
        return javaClass.hashCode()
    }

}

class AdState(id : Int, name : String, inactivate : Boolean, automate : Boolean, confuse : Boolean, reflect : Boolean,
              dur : Int = 3, sRes : Int = 0, dmgHp : Int, dmgMp : Int, dmgSp : Int, mHp : Int, mMp : Int, mSp : Int, mAtk : Int,
              mDef: Int, mSpi: Int, mWis : Int, mAgi : Int, mActions : Int, range: Boolean, mRes : MutableMap<Int, Int>? = null,
              skills : Array<Ability>? = null, rSkills : Array<Ability>? = null, rStates : Array<State>? = null,
              mStRes : MutableMap<State, Int>? = null, mInit: Int = 0)
    : State(id, name, inactivate, automate, confuse, reflect, dur, sRes, dmgHp, dmgMp, dmgSp, mHp, mMp, mSp, mAtk, mDef, mSpi,
        mWis, mAgi, mActions, mInit, range, mRes, skills, rSkills, rStates, mStRes) {

    override fun equals(other: Any?): Boolean {
        return super.equals(other) || (other is AdState && other.id == this.id)
    }

    override fun hashCode(): Int {
        return javaClass.hashCode()
    }

}

val AnimationDrawable.fullDur : Int
get() {
    var s = 0
    for (i in 0 until this.numberOfFrames) {
        s += getDuration(i)
    }
    return s
}

fun AnimationDrawable.getInvertedSprite(firstFrameWait: Boolean) : AnimationDrawable {
    val animSpr = AnimationDrawable()
    animSpr.isOneShot = true
    for (i in this.numberOfFrames - 1 downTo 0) {
        animSpr.addFrame(this.getFrame(i), if (firstFrameWait && i == this.numberOfFrames - 1) 261 else 87)
    }
    return animSpr
}

fun BitmapDrawable.getSprite(context : Context, firstFrame : Drawable? = null, firstFrameWait : Boolean,
                             lastFrame : Drawable? = null, addPlayback : Boolean) : AnimationDrawable {
    val bmp: Bitmap = this.bitmap
    val animSpr = AnimationDrawable()
    animSpr.isOneShot = true
    if (firstFrameWait && firstFrame !== null) {
        animSpr.addFrame(firstFrame, 261)
    }
    val sprCount = bmp.width / bmp.height
    val sprWidth = bmp.width / sprCount
    val lastSprDur = if (addPlayback && sprCount < 7) 261 else 87
    for (i in 0 until sprCount) {
        animSpr.addFrame(BitmapDrawable(context.resources,
                Bitmap.createBitmap(bmp, i * sprWidth, 0, sprWidth, bmp.height)),
                if (i < sprCount - 1) 87 else lastSprDur)
    }
    if (lastSprDur == 261) {
        for (i in sprCount - 2 downTo 1) {
            animSpr.addFrame(animSpr.getFrame(i), 87)
        }
    }
    if (lastFrame !== null) {
        animSpr.addFrame(lastFrame, 1)
    }
    //bmp.recycle()
    return animSpr
}

class Arena : Fragment() {

    private inner class AdScene(party : Array<Actor>, enemy : Array<Actor>, surprise: Int) : Scene(party, enemy, surprise) {

        override fun executeAbility(skill: Ability, defTarget: Int, txt: String): String {
            val jScripts = this@Arena.jScripts
            if (jScripts !== null) {
                val jsContext = this@Arena.jsContext
                val jsScope = this@Arena.jsScope
                if (jsContext !== null && jsScope !== null) {
                    jsScope.put("Target", jsScope, org.mozilla.javascript.Context.javaToJS(defTarget, jsScope))
                    jsScope.put("Ability", jsScope, org.mozilla.javascript.Context.javaToJS(skill, jsScope))
                    if (jScripts.size > 2 && jScripts[2] !== null) {
                        try {
                            jsContext.evaluateString(jsScope, jScripts[2], "BeforeAct", 1, null)
                        } catch (e: Exception) {
                            Log.e("Rhino", e.message)
                        }
                    }
                }
            }
            return super.executeAbility(skill, defTarget, txt)
        }

        override fun setNextCurrent(): String {
            val jScripts = this@Arena.jScripts
            if (jScripts !== null && jScripts.isNotEmpty()) {
                var jsContext = this@Arena.jsContext
                var jsScope = this@Arena.jsScope
                if (jsContext === null || jsScope === null) {
                    jsContext = org.mozilla.javascript.Context.enter()
                    jsContext.optimizationLevel = -1
                    jsScope = jsContext.initSafeStandardObjects()
                    jsScope.put("Arena", jsScope, org.mozilla.javascript.Context.javaToJS(this@Arena, jsScope))
                    jsScope.put("Scene", jsScope, org.mozilla.javascript.Context.javaToJS(this, jsScope))
                    jsScope.put("Players", jsScope, org.mozilla.javascript.Context.javaToJS(this.players, jsScope))
                    jsScope.put("EnemyIndex", jsScope, org.mozilla.javascript.Context.javaToJS(this.enIdx, jsScope))
                    if (jScripts[0] !== null) {
                        try {
                            jsContext.evaluateString(jsScope, jScripts[0], "OnStart", 1, null)
                        } catch (e: Exception) {
                            Log.e("Rhino", e.message)
                        }
                    }
                    this@Arena.jsContext = jsContext
                    this@Arena.jsScope = jsScope
                }
                jsScope!!.put("Current", jsScope, org.mozilla.javascript.Context.javaToJS(this.current, jsScope))
                if (jScripts.size > 1 && jScripts[1] !== null) {
                    try {
                        jsContext!!.evaluateString(jsScope, jScripts[1], "OnBeginTurn", 1, null)
                    } catch (e: Exception) {
                        Log.e("Rhino", e.message)
                    }
                }
            }
            return super.setNextCurrent()
        }

    }

    private class ActorArrayBinder(val actorArray : Array<Actor>) : Binder()

    companion object {
        fun prepare(arenaImgId : Int, songId : Int, party : Array<Actor>, enemy : Array<Actor>, surprise : Int, escapable: Boolean, scripts: Array<String?>?): Arena {
            val arena = Arena()
            val actBundle = Bundle()
            actBundle.putBinder("party", ActorArrayBinder(party))
            actBundle.putBinder("enemy", ActorArrayBinder(enemy))
            actBundle.putInt("surprise", surprise)
            actBundle.putBoolean("escapable", escapable)
            actBundle.putStringArray("scripts", scripts)
            actBundle.putInt("arenaImg", arenaImgId)
            actBundle.putInt("song", songId)
            //activity.startActivityForResult(Intent(activity, Arena::class.java).putExtras(actBundle), 1)
            arena.arguments = actBundle
            return arena
        }
    }

    private lateinit var scenePlay : AdScene

    private lateinit var imgActor : Array<ImageView>
    private lateinit var autoBtn : Button
    private lateinit var runBtn : Button
    private lateinit var skillActBtn : Button
    private lateinit var itemUseBtn : Button
    private lateinit var skillsSpn : Spinner
    private lateinit var itemsSpn : Spinner
    private lateinit var targetSpn : Spinner
    private lateinit var actionsTxt : TextView
    private lateinit var infoTxt : TextView

    private lateinit var songPlayer : MediaPlayer

    private var koActors : Int = 0

    private var partySide = 0
    private var otherSide = 1

    private var automatic = false
    private var escapable = true

    private var skillsAdapter : AbilityArrayAdapter? = null
    private lateinit var playersAdapter : ActorArrayAdapter
    private var itemsAdapter : AbilityArrayAdapter? = null

    private var jScripts : Array<String?>? = null
    private var jsContext : org.mozilla.javascript.Context? = null
    private var jsScope : Scriptable? = null

    private val cAction = View.OnClickListener {
        when (it.id) {
            R.id.ActBt -> {
                this.enableControls(false)
                this.actionsTxt.append(this.scenePlay.performSkill(this.skillsSpn.selectedItemPosition,
                        this.targetSpn.selectedItemPosition, ""))
                this.playSpr()
            }
            R.id.AutoBt -> {
                this.automatic = !this.automatic
                if (this.automatic) {
                    this.enableControls(false)
                    if (this.crActor.automatic == 0) {
                        this.afterAct()
                    }
                }
                else {
                    this.autoBtn.isEnabled = false
                }
            }
            R.id.UseBt -> {
                val itemsAdapter = this.itemsAdapter
                if (itemsAdapter !== null) {
                    this.enableControls(false)
                    this.actionsTxt.append(this.scenePlay.useItem(this.itemsSpn.selectedItemPosition,
                            this.targetSpn.selectedItemPosition, ""))
                    itemsAdapter.notifyDataSetChanged()
                    this.playSpr()
                }
            }
            R.id.RunBt -> {
                val escText = this.scenePlay.escape()
                this.actionsTxt.append(this.scenePlay.endTurn(""))
                this.actionsTxt.append("\n$escText")
                if (this.scenePlay.status == -1) {
                    this.endingMsg(this.getString(R.string.escape), Scene.escapeTxt)
                }
                else {
                    this.enableControls(false)
                    this.afterAct()
                }
            }
        }
    }

    private class ViewHolder(var nameText : TextView) {
        var usable : Boolean = true
    }

    private class AbilityArrayAdapter(context: Arena, val layoutRes: Int, skills: List<Ability>, val asItems : Boolean)
        : ArrayAdapter<Ability>(context.requireContext(), layoutRes) {

        var arenaAct = context

        var skills = skills
        set(value) {
            field = value
            this.notifyDataSetChanged()
        }

        override fun getItem(position: Int): Ability {
            return this.skills[position]
        }

        override fun getCount(): Int {
            return this.skills.size
        }

        private fun prepareView(position: Int, convertView: View?, parent: ViewGroup?): View {
            val view : View
            val vHolder : ViewHolder
            if (convertView === null || convertView.tag === null) {
                view = (this.context.getSystemService(Context.LAYOUT_INFLATER_SERVICE) as LayoutInflater)
                        .inflate(this.layoutRes, parent, false)
                val txt = view.findViewById<TextView>(android.R.id.text1)
                vHolder = ViewHolder(txt)
                view.tag = vHolder
            }
            else {
                view = convertView
                vHolder = convertView.tag as ViewHolder
            }

            vHolder.usable = this.skills[position].canPerform(this.arenaAct.crActor)
            vHolder.nameText.setTextColor(if (vHolder.usable) Color.WHITE else Color.GRAY)
            return view
        }

        override fun getDropDownView(position: Int, convertView: View?, parent: ViewGroup?): View {
            //return this.getView(position, convertView, parent)
            val view = this.prepareView(position, convertView, parent)
            val vHolder = view.tag as ViewHolder
            val skill = this.skills[position]
            vHolder.nameText.text = skill.name +
                    (if (this.asItems) " x ${this.arenaAct.crActor._items?.get(skill)} " else " ") +
                    String.format(this.context.getString(R.string.skill_info), skill.lvRq, skill.hpC,
                            skill.mpC, skill.spC, (this.arenaAct.crActor.skillsQty?.get(skill) ?: "∞"),
                            (when {
                                skill.trg == 0 -> this.context.getString(R.string.one)
                                skill.trg == -1 -> this.context.getString(R.string.self)
                                else -> this.context.getString(R.string.all)
                            }),
                            if (skill.range == true) this.context.getString(R.string.yes)
                            else this.context.getString(R.string.no))
            return view
        }

        override fun getView(position: Int, convertView: View?, parent: ViewGroup?): View {
            val view = this.prepareView(position, convertView, parent)
            val vHolder = view.tag as ViewHolder
            val skill = this.skills[position]
            vHolder.nameText.text = if (this.asItems) "${skill.name} x ${this.arenaAct.crActor._items?.get(skill)} " else skill.name
            return view
        }
    }

    private class ActorArrayAdapter(context: Arena, val layoutRes: Int, actors: Array<Actor>)
        : ArrayAdapter<Actor>(context.requireContext(), layoutRes) {

        var arenaAct = context

        var actors = actors
        set(value) {
            field = value
            this.notifyDataSetChanged()
        }

        override fun getItem(position: Int): Actor {
            return this.actors[position]
        }

        override fun getCount(): Int {
            return this.actors.size
        }

        private fun prepareView(convertView: View?, parent: ViewGroup?): View {
            val view : View
            if (convertView === null || convertView.tag === null) {
                view = (this.context.getSystemService(Context.LAYOUT_INFLATER_SERVICE) as LayoutInflater)
                        .inflate(this.layoutRes, parent, false)
                val txt = view.findViewById<TextView>(android.R.id.text1)
                view.tag = ViewHolder(txt)
            }
            else {
                view = convertView
            }
            return view
        }

        override fun getDropDownView(position: Int, convertView: View?, parent: ViewGroup?): View {
            val view = this.prepareView(convertView, parent)
            val vHolder = view.tag as ViewHolder
            val actor = this.actors[position]
            vHolder.nameText.text = "${actor.name} (${Costume.hpText}: " +
                    ((if (position < this.arenaAct.scenePlay.enIdx)
                        "${actor.hp}/${actor.mHp}, ${Costume.mpText}: ${actor.mp}/${actor.mMp}, ${Costume.spText}: ${actor.sp}/${actor.mSp}"
                    else "%.2f".format((actor.hp.toFloat() / actor.mHp.toFloat()) * 100.0f) + "%") + ")")
            return view
        }

        override fun getView(position: Int, convertView: View?, parent: ViewGroup?): View {
            val view = this.prepareView(convertView, parent)
            val vHolder = view.tag as ViewHolder
            vHolder.nameText.text = this.actors[position].name
            return view
        }
    }

    private val crActor : AdActor
        get() {
            return this.scenePlay.players[this.scenePlay.current] as AdActor
        }

    private fun canTarget(target : Int, ability : Ability) : Boolean {
        return this.scenePlay.getGuardian(target, ability) == target
                && (this.scenePlay.players[target].hp > 0 || ability.restoreKO)
    }

    private fun enableControls(enable : Boolean) {
        if (!enable) {
            this.skillActBtn.isEnabled = false
            this.itemUseBtn.isEnabled = false
            this.itemsSpn.isEnabled = false
        }
        this.skillsSpn.isEnabled = enable
        if (this.escapable) {
            this.runBtn.isEnabled = enable
        }
    }

    private fun setCrAutoSkill() {
        val onPartySide = this.targetSpn.selectedItemPosition < this.scenePlay.enIdx
        val autoSkill = this.scenePlay.getAIskill(if (onPartySide) 1 else 0,
                onPartySide && this.scenePlay.players[this.targetSpn.selectedItemPosition].hp < 1)
        if (this.skillsSpn.selectedItemPosition == autoSkill) {
            this.skillActBtn.isEnabled = (this.crActor.availableSkills[autoSkill]).canPerform(this.crActor)
                    && this.canTarget(this.targetSpn.selectedItemPosition, this.crActor.availableSkills[autoSkill])
        }
        else {
            this.skillsSpn.setSelection(autoSkill)
        }
    }

    private fun afterAct() {
        val jScripts = this.jScripts
        if (jScripts !== null) {
            val jsContext = this.jsContext
            val jsScope = this.jsScope
            if (jsContext !== null && jsScope !== null) {
                jsScope.put("FirstTarget", jsScope, org.mozilla.javascript.Context.javaToJS(this.scenePlay.fTarget, jsScope))
                jsScope.put("LastTarget", jsScope, org.mozilla.javascript.Context.javaToJS(this.scenePlay.lTarget, jsScope))
                jsScope.put("Outcome", jsScope, org.mozilla.javascript.Context.javaToJS(this.scenePlay.status, jsScope))
                if ( jScripts.size > 3 && jScripts[3] !== null) {
                    try {
                        jsContext.evaluateString(jsScope, jScripts[3], "AfterAct", 1, null)
                    } catch (e: Exception) {
                        Log.e("Rhino", e.message)
                    }
                }
            }
        }
        when (this.scenePlay.status) {
            0 -> {
                if (this.automatic || this.crActor.automatic != 0) {
                    if (this.infoTxt.text.isNotEmpty()) {
                        this.infoTxt.text = ""
                    }
                    this.actionsTxt.append(this.scenePlay.executeAI(""))
                    this.playSpr()
                }
                else {
                    this.setCrSkills()
                    this.setCrItems()
                    val actor = this.crActor
                    this.infoTxt.text = String.format(this.getString(R.string.cr_actor_info), actor.name, actor.level, actor.exp, actor.mExp)
                    this.enableControls(true)
                    this.autoBtn.isEnabled = true
                    this.setCrAutoSkill()
                }
            }
            1 -> this.endingMsg(this.getString(R.string.victory), Scene.victoryTxt)
            -2 -> this.endingMsg(this.getString(R.string.defeat), Scene.fallenTxt)
        }
    }

    private fun playSpr() {
        val scenePlay = this.scenePlay
        val imgActor = this.imgActor
        val current = scenePlay.current
        val lastAbility = scenePlay.lastAbility as AdAbility?
        val sprType = if (lastAbility === null || lastAbility.trg < 0
                || lastAbility.dmgType == 2 || lastAbility.dmgType == 3) 6 else 5
        val usrSide = if (current < scenePlay.enIdx) this.partySide else this.otherSide
        val crActor = this.crActor
        val actAnim = crActor.getBtSprite(usrSide, sprType)//crActor.sprites[usrSide][sprType]
        var dur = crActor.spritesDur[usrSide][sprType] - 174
        if (actAnim !== null) {
            actAnim.stop()
            imgActor[current].setBackgroundDrawable(actAnim)
            actAnim.start()
        }
        imgActor[current].postDelayed({
            var htActor: AdActor
            for (trg in scenePlay.fTarget..scenePlay.lTarget) {
                if (trg != current && (lastAbility === null
                                || !(lastAbility.dmgType == 2 && scenePlay.players[trg].reflect))) {
                    htActor = (scenePlay.players[trg] as AdActor)
                    val trgAnim: Int
                    val koActors = this.koActors
                    val koBit = 1 shl trg
                    if (htActor.hp > 0) {
                        if (koActors and koBit == koBit) {
                            trgAnim = 4
                            this.koActors = koActors - koBit
                        } else {
                            trgAnim = 2
                        }
                    } else {
                        if (koActors and koBit == koBit) continue
                        trgAnim = 3
                        this.koActors = koActors + koBit
                    }
                    if (lastAbility !== null) {
                        val abilitySpr = lastAbility.getSprite(this.requireContext())
                        if (abilitySpr !== null) {
                            if (lastAbility.spriteDur > dur) {
                                dur = lastAbility.spriteDur
                            }
                            abilitySpr.stop()
                            imgActor[trg].setImageDrawable(abilitySpr)
                            abilitySpr.start()
                        }
                        val soundDur = lastAbility.playSound(this.requireContext())
                        if (soundDur > dur) {
                            dur = soundDur
                        }
                    }
                    val trgSide = if (trg < scenePlay.enIdx) this.partySide else this.otherSide
                    val hitAnim = htActor.getBtSprite(trgSide, trgAnim)//htActor.sprites[trgSide][trgAnim]
                    if (hitAnim !== null) {
                        hitAnim.stop()
                        if (htActor.spritesDur[trgSide][trgAnim] > dur) {
                            dur = htActor.spritesDur[trgSide][trgAnim]
                        }
                        imgActor[trg].setBackgroundDrawable(hitAnim)
                        hitAnim.start()
                    }
                }
            }
            imgActor[current].postDelayed({
                if (crActor.hp < 1) {
                    this.koActors += (1 shl current)
                    val fallAnim = crActor.getBtSprite(usrSide, 3)//crActor.sprites[usrSide][3]
                    if (fallAnim !== null) {
                        fallAnim.stop()
                        imgActor[current].setBackgroundDrawable(fallAnim)
                        fallAnim.start()
                    }
                }
                this.actionsTxt.append(scenePlay.endTurn(""))
                this.afterAct()
            }, dur.toLong())
        }, 174)
    }

    private fun setCrItems() {
        val crItems = this.scenePlay.crItems!![this.scenePlay.current]
        if (crItems === null || crItems.isEmpty()) {
            if (this.itemsSpn.isEnabled) {
                this.itemsSpn.setSelection(Spinner.INVALID_POSITION)
                this.itemsSpn.isEnabled = false
            }
        }
        else {
            var itemsAdapter = this.itemsAdapter
            if (itemsAdapter === null) {
                itemsAdapter = AbilityArrayAdapter(this, android.R.layout.simple_spinner_dropdown_item, crItems, true)
                this.itemsAdapter = itemsAdapter
                this.itemsSpn.adapter = itemsAdapter

                this.itemsSpn.onItemSelectedListener = object : AdapterView.OnItemSelectedListener {
                    override fun onNothingSelected(parent: AdapterView<*>?) {
                        this@Arena.itemUseBtn.isEnabled = false
                    }

                    override fun onItemSelected(parent: AdapterView<*>?, view: View?, position: Int, id: Long) {
                        this@Arena.itemUseBtn.isEnabled = (view?.tag as ViewHolder).usable
                                && this@Arena.canTarget(this@Arena.targetSpn.selectedItemPosition, this@Arena.itemsSpn.selectedItem as Ability)
                    }

                }

                this.itemUseBtn.setOnClickListener(this.cAction)
            }
            else if (itemsAdapter.skills !== crItems) {
                itemsAdapter.skills = crItems

            }
            if (!this.itemsSpn.isEnabled) {
                this.itemsSpn.setSelection(0)
                this.itemsSpn.isEnabled = true
            }
            this.targetSpn.selectedItemPosition
            if (this.targetSpn.selectedItemPosition > -1) {
                val item = this.itemsSpn.selectedItem as Ability
                this.itemUseBtn.isEnabled = item.canPerform(this.crActor) && this.canTarget(this.targetSpn.selectedItemPosition, item)
            }
        }
    }

    private fun setCrSkills() {
        var skillsAdapter = this.skillsAdapter
        if (skillsAdapter === null) {
            skillsAdapter = AbilityArrayAdapter(this, android.R.layout.simple_spinner_dropdown_item,
                    this.crActor.availableSkills, false)
            this.skillsSpn.adapter = skillsAdapter
            this.skillsAdapter = skillsAdapter

            this.skillsSpn.onItemSelectedListener = object : AdapterView.OnItemSelectedListener {
                override fun onNothingSelected(parent: AdapterView<*>?) {
                    this@Arena.skillActBtn.isEnabled = false
                }

                override fun onItemSelected(parent: AdapterView<*>?, view: View?, position: Int, id: Long) {
                    this@Arena.skillActBtn.isEnabled = ((view !== null && (view.tag as ViewHolder).usable)
                            || (view === null && this@Arena.crActor.availableSkills[position].canPerform(this@Arena.crActor)))
                            && this@Arena.canTarget(this@Arena.targetSpn.selectedItemPosition, this@Arena.crActor.availableSkills[position])
                }

            }
            this.skillActBtn.setOnClickListener(this.cAction)
        }
        else {
            skillsAdapter.skills = this.crActor.availableSkills
        }
    }

    private fun ImageView.setTargetClickListener(targetPos : Int) {
        this.setOnClickListener {
            if (targetPos == this@Arena.targetSpn.selectedItemPosition) {
                if (this@Arena.crActor.automatic == 0 && this@Arena.skillActBtn.isEnabled) {
                    this@Arena.skillActBtn.callOnClick()
                }
            }
            else {
                this@Arena.targetSpn.setSelection(targetPos)
            }
        }
    }

    private fun endingMsg(t: String, s: String) {
        val context = this.context
        if (context !== null) {
            AlertDialog.Builder(context)
                    .setCancelable(false)
                    .setMessage(s)
                    .setTitle(t).setPositiveButton(this.resources.getString(R.string.exit)) { _, _ ->
                        val jScripts = this.jScripts
                        if (jScripts !== null && jScripts.size > 4 && jScripts[4] !== null) {
                            val jsContext = this.jsContext
                            val jsScope = this.jsScope
                            if (jsContext !== null && jsScope !== null) {
                                jsScope.delete("Arena")
                                try {
                                    jsContext.evaluateString(jsScope, jScripts[4], "OnStop", 1, null)
                                } catch (e: Exception) {
                                    Log.e("Rhino", e.message)
                                }
                                org.mozilla.javascript.Context.exit()
                            }
                        }
                        //val outcome = Intent()
                        //outcome.putExtra("Outcome", this.scenePlay.status)
                        //this.requireActivity().setResult(Activity.RESULT_OK, outcome)
                        //this.requireActivity().supportFragmentManager.beginTransaction().remove(this).commit()
                        if (context is ArenaStager) {
                            context.onArenaConclusion(this.scenePlay.status)
                        }
                    }.create().show()
        }
    }

    fun renounce() {
        this.scenePlay.status = -2
        this.afterAct()
    }

    override fun onCreateView(inflater: LayoutInflater, container: ViewGroup?, savedInstanceState: Bundle?): View? {
        //super.onCreateView(inflater, container, savedInstanceState)
        //this.setContentView(R.layout.activity_arena)
        val view = inflater.inflate(R.layout.activity_arena, container, false)

        val extra = this.arguments
        val party : Array<Actor>
        val enemy : Array<Actor>
        val surprised : Int
        if (extra !== null) {
            surprised = extra.getInt("surprise", 0)
            this.escapable = extra.getBoolean("escapable", true)
            val songResId = extra.getInt("song", 0)
            if (songResId > 0) {
                this.songPlayer = MediaPlayer.create(this.context, songResId)
                this.songPlayer.isLooping = true
                this.songPlayer.start()
            }
            val arenaResId = extra.getInt("arenaImg", 0)
            if (arenaResId > 0) {
                view.findViewById<ImageView>(R.id.ImgArena).setBackgroundResource(arenaResId)
            }
            party = (extra.getBinder("party") as ActorArrayBinder).actorArray
            enemy = (extra.getBinder("enemy") as ActorArrayBinder).actorArray
            val jScripts = extra.getStringArray("scripts")
            if (jScripts !== null) {
                this.jScripts = jScripts
            }
        }
        else {
            surprised = 0

            val humanRace = Costume(1, "Human")
            val heroJob = AdCostume(1, "Hero", "hero")
            val valkyrieJob = AdCostume(1, "Valkyrie", "valkyrie")
            val crusaderJob = AdCostume(1, "Crusader", "crusader")
            val sorceressJob = AdCostume(1, "Sorceress", "sorceress")
            val ninjaJob = AdCostume(1, "Ninja", "ninja")
            val dragoonJob = AdCostume(1, "Dragoon", "dragoon")
            val hesychastJob = AdCostume(1, "Hesychast", "hesychast")
            val shamanJob = AdCostume(1, "Shaman", "shaman")
            val alchemistJob = AdCostume(1, "Alchemist", "alchemist")
            val reaverJob = AdCostume(1, "Reaver", "reaver")
            val rangerJob = AdCostume(1, "Ranger", "ranger")
            val corsairJob = AdCostume(1, "Corsair", "corsair")
            val druidJob = AdCostume(1, "Druid", "druid")
            val knightJob = AdCostume(1, "Knight", "knight")
            val spyJob = AdCostume(1, "Spy", "spy")
            val wizardJob = AdCostume(1, "Wizard", "wizard")
            val berserkerJob = AdCostume(1, "Berserker", "berserker")
            val ogreJob = AdCostume(1, "Ogre", "ogre")
            val lizardJob = AdCostume(1, "Lizard", "lizard")
            val trollJob = AdCostume(1, "Troll", "troll")
            val goblinJob = AdCostume(1, "Goblin", "goblin")

            val skills: Array<Ability> = arrayOf(
                    AdAbility(1, "Attack", 0, 0, false, null, 1, 0, 0, 1, 10, 0, 0,
                            0, 0, 0, 0, 0, 0, false, false, null, null),
                    AdAbility(2, "Defend", 0, 0, false, false, 1, 0, 0, 0, 0, -2, -3,
                            1, 0, -1, 0, 0, 0, false, false, null, null),
                    AdAbility(3, "Heal", 0, 0, true, false, 1, 0, 3, 0, -15, 0, 0,
                            3, 0, 0, 0, 0, 0, false, true, null, null)
            )

            val skills2: Array<Ability> = arrayOf(
                    AdAbility(1, "Act", 0, 0, false, null, 1, 0, 0, 0, 10, 0, 0,
                            0, 0, 0, 0, 0, 0, false, false, null, null),
                    AdAbility(2, "Guard", 0, 0, false, false, 1, 0, 0, 0, 0, -2, -3,
                            1, 0, -1, 0, 0, 0, false, false, null, null),
                    AdAbility(1, "Hit", 0, 0, false, false, 1, 0, 45, 1, 30, 0, 0,
                            0, 0, 0, 0, 0, 0, false, false, null, null))

            party = arrayOf(
                    AdActor(1, this.requireContext(), "Cody", null, humanRace, knightJob, 1, 9, 1, 50, 25, 25, 7, 7,
                            7, 7, 7, false, null, skills2, null, null),
                    AdActor(2, this.requireContext(), "Victoria", null, humanRace, valkyrieJob, 1, 9, 1, 50, 25, 25, 7, 7,
                            7, 7, 7, false, null, skills2, null, null),
                    AdActor(3, this.requireContext(), "Stephanie", null, humanRace, sorceressJob, 1, 9, 1, 50, 25, 25, 7, 7,
                            7, 7, 7, false, null, skills, null, null),
                    AdActor(4, this.requireContext(), "George", null, humanRace, hesychastJob, 1, 9, 1, 50, 25, 25, 7, 7,
                            7, 7, 7, false, null, skills, null, null)
            )
            party[0]._items = LinkedHashMap()
            val potion = AdAbility(10, "Potion", 0, 0, false, false, 1, 0, 3, 0, -15, 0, 0,
                    3, 0, 0, 0, 0, 0, false, false, null, null)
            party[0]._items!![potion] = 3
            party[1]._items = party[0]._items

            enemy = arrayOf(
                    AdActor(8, this.requireContext(), "Goblin", null, humanRace, goblinJob, 1, 9, 1, 50, 25, 25, 7, 7,
                            7, 7, 7, false, null, skills, null, null),
                    AdActor(7, this.requireContext(), "Troll", null, humanRace, trollJob, 1, 9, 1, 50, 25, 25, 7, 7,
                            7, 7, 7, false, null, skills, null, null),
                    AdActor(6, this.requireContext(), "Lizard", null, humanRace, lizardJob, 1, 9, 1, 50, 25, 25, 7, 7,
                            7, 7, 7, false, null, skills, null, null),
                    AdActor(5, this.requireContext(), "Ogre", null, humanRace, ogreJob, 1, 9, 1, 50, 25, 25, 7, 7,
                            7, 7, 7, false, null, skills, null, null)
            )
        }

        if (surprised < 0) {
            this.partySide = 1
            this.otherSide = 0
        }
        else {
            this.partySide = 0
            this.otherSide = 1
        }

        this.scenePlay = AdScene(party, enemy, surprised)

        this.runBtn = view.findViewById(R.id.RunBt)
        this.runBtn.isEnabled = this.escapable
        this.autoBtn = view.findViewById(R.id.AutoBt)
        this.skillActBtn = view.findViewById(R.id.ActBt)
        this.itemUseBtn = view.findViewById(R.id.UseBt)
        this.skillsSpn = view.findViewById(R.id.SkillBox)
        this.itemsSpn = view.findViewById(R.id.ItemBox)
        this.targetSpn = view.findViewById(R.id.TargetBox)
        this.actionsTxt = view.findViewById(R.id.ItemCost)
        this.actionsTxt.movementMethod = ScrollingMovementMethod()
        this.infoTxt = view.findViewById(R.id.SkillCost)

        var koActors = this.koActors
        this.scenePlay.players.forEachIndexed { i, actor ->
            if (actor.hp < 1) {
                koActors += 1 shl i
            }
        }
        this.koActors = koActors

        val imgViews = ArrayList<ImageView>(party.size + enemy.size)

        var pos = 0
        var imgView : ImageView?
        if (surprised < 0) {
            if (party.isNotEmpty()) {
                imgView = view.findViewById(R.id.ImgEnemy1)
                imgView.setTargetClickListener(pos++)
                imgViews.add(imgView)
            }
            if (party.size > 1) {
                imgView = view.findViewById(R.id.ImgEnemy2)
                imgView.setTargetClickListener(pos++)
                imgViews.add(imgView)
            }
            if (party.size > 2) {
                imgView = view.findViewById(R.id.ImgEnemy3)
                imgView.setTargetClickListener(pos++)
                imgViews.add(imgView)
            }
            if (party.size > 3) {
                imgView = view.findViewById(R.id.ImgEnemy4)
                imgView.setTargetClickListener(pos++)
                imgViews.add(imgView)
            }
            if (enemy.isNotEmpty()) {
                imgView = view.findViewById(R.id.ImgPlayer1)
                imgView.setTargetClickListener(pos++)
                imgViews.add(imgView)
            }
            if (enemy.size > 1) {
                imgView = view.findViewById(R.id.ImgPlayer2)
                imgView.setTargetClickListener(pos++)
                imgViews.add(imgView)
            }
            if (enemy.size > 2) {
                imgView = view.findViewById(R.id.ImgPlayer3)
                imgView.setTargetClickListener(pos++)
                imgViews.add(imgView)
            }
            if (enemy.size > 3) {
                imgView = view.findViewById(R.id.ImgPlayer4)
                imgView.setTargetClickListener(pos)
                imgViews.add(imgView)
            }
        }
        else {
            if (party.isNotEmpty()) {
                imgView = view.findViewById(R.id.ImgPlayer1)
                imgView.setTargetClickListener(pos++)
                imgViews.add(imgView)
            }
            if (party.size > 1) {
                imgView = view.findViewById(R.id.ImgPlayer2)
                imgView.setTargetClickListener(pos++)
                imgViews.add(imgView)
            }
            if (party.size > 2) {
                imgView = view.findViewById(R.id.ImgPlayer3)
                imgView.setTargetClickListener(pos++)
                imgViews.add(imgView)
            }
            if (party.size > 3) {
                imgView = view.findViewById(R.id.ImgPlayer4)
                imgView.setTargetClickListener(pos++)
                imgViews.add(imgView)
            }
            if (enemy.isNotEmpty()) {
                imgView = view.findViewById(R.id.ImgEnemy1)
                imgView.setTargetClickListener(pos++)
                imgViews.add(imgView)
            }
            if (enemy.size > 1) {
                imgView = view.findViewById(R.id.ImgEnemy2)
                imgView.setTargetClickListener(pos++)
                imgViews.add(imgView)
            }
            if (enemy.size > 2) {
                imgView = view.findViewById(R.id.ImgEnemy3)
                imgView.setTargetClickListener(pos++)
                imgViews.add(imgView)
            }
            if (enemy.size > 3) {
                imgView = view.findViewById(R.id.ImgEnemy4)
                imgView.setTargetClickListener(pos)
                imgViews.add(imgView)
            }
            //this.setCrSkills()
            //this.setCrItems()
        }

        this.imgActor = imgViews.toTypedArray()

        for (i in 0 until this.scenePlay.enIdx) {
            this.imgActor[i].setBackgroundDrawable((this.scenePlay.players[i] as AdActor).getBtSprite(this.partySide,
                    if (koActors and (1 shl i) == 1 shl i) 1 else 0))
        }

        for (i in this.scenePlay.enIdx until this.scenePlay.players.size) {
            this.scenePlay.players[i].automatic = 2
            this.imgActor[i].setBackgroundDrawable((this.scenePlay.players[i] as AdActor).getBtSprite(this.otherSide,
                    if (koActors and (1 shl i) == 1 shl i) 1 else 0))
        }

        this.playersAdapter = ActorArrayAdapter(this, android.R.layout.simple_spinner_dropdown_item,
                this.scenePlay.players)

        this.targetSpn.adapter = this.playersAdapter

        this.targetSpn.setSelection(this.scenePlay.enIdx)

        this.targetSpn.onItemSelectedListener = object : AdapterView.OnItemSelectedListener {

            override fun onNothingSelected(parent: AdapterView<*>?) {
                this@Arena.skillActBtn.isEnabled = false
                this@Arena.itemUseBtn.isEnabled = false
            }

            override fun onItemSelected(parent: AdapterView<*>?, view: View?, position: Int, id: Long) {
                if (this@Arena.crActor.automatic == 0) {
                    if (this@Arena.itemsSpn.isEnabled) {
                        this@Arena.itemUseBtn.isEnabled = (this@Arena.itemsSpn.selectedView.tag as ViewHolder).usable
                                && this@Arena.canTarget(position, this@Arena.itemsSpn.selectedItem as Ability)
                    }
                    if (this@Arena.skillsSpn.isEnabled) {
                        this@Arena.setCrAutoSkill()
                    }
                }
            }
        }

        this.runBtn.setOnClickListener(this.cAction)
        this.autoBtn.setOnClickListener(this.cAction)

        /*if (this.crActor.automatic != 0) {
            this.afterAct()
        }
        else {
            this.enableControls(true)
            this.setCrAutoSkill()
        }*/
        return view
    }

    override fun onStart() {
        super.onStart()
        if (this.scenePlay.players[this.scenePlay.current].automatic != 0) {
            this.enableControls(false)
        }
        this.afterAct()
    }

}

interface ArenaStager {

    fun onArenaConclusion(outcome: Int)

    fun stage(activity: AppCompatActivity, @IdRes fragmentResId: Int, @DrawableRes arenaImgId: Int, @RawRes songId: Int,
              party: Array<Actor>, enemy: Array<Actor>, surprise: Int, escapable: Boolean, scripts: Array<String?>?): Arena {
        val arena = Arena.prepare(arenaImgId, songId, party, enemy, surprise, escapable, scripts)
        activity.supportFragmentManager.beginTransaction().add(fragmentResId, arena).commit()
        return arena
    }

}