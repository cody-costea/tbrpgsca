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
package com.codycostea.tbrpgsca.library

abstract class Role(val id: Int, open var name: String, open var mHp: Int = 30, open var mMp: Int = 10, open var mSp: Int = 10,
                    open var mInit: Int = 0, range: Boolean = false, open var aStates: Array<State>? = null) {

    companion object {
        @JvmStatic
        var hpText = "HP"

        @JvmStatic
        var mpText = "MP"

        @JvmStatic
        var spText = "RP"

        const val FLAG_RANGE: Int = 1

        fun getDmgText(dmgHp: Int, dmgMp: Int, dmgSp: Int): String {
            var s = ""
            var c = false
            if (dmgHp != 0) {
                s += " "
                if (dmgHp < 0) {
                    s += "+"
                }
                s += "${-dmgHp} $hpText"
                c = true
            }
            if (dmgMp != 0) {
                if (c) {
                    s += ","
                }
                s += " "
                if (dmgMp < 0) {
                    s += "+"
                }
                s += "${-dmgMp} $mpText"
                c = true
            }
            if (dmgSp != 0) {
                if (c) {
                    s += ","
                }
                s += " "
                if (dmgSp < 0) {
                    s += "+"
                }
                s += "${-dmgSp} $spText"
            }
            return s
        }
    }

    protected var flags: Int

    open var range: Boolean
        get() {
            return (this.flags and FLAG_RANGE) == FLAG_RANGE
        }
        set(value) {
            val flags = this.flags
            if (value != (flags and FLAG_RANGE == FLAG_RANGE)) {
                this.flags = flags xor FLAG_RANGE
            }
        }

    override fun equals(other: Any?): Boolean {
        return super.equals(other) || (other is Role && other.id == this.id)
    }

    override fun hashCode(): Int {
        return javaClass.hashCode()
    }

    init {
        this.flags = if (range) FLAG_RANGE else 0
    }

}

open class Costume(id: Int, name: String, open var sprite: String? = null, mHp: Int = 30, mMp: Int = 10, mSp: Int = 10, open var atk: Int = 7,
                   open var def: Int = 7, open var spi: Int = 7, open var wis: Int = 7, open var agi: Int = 7, open var mActions: Int = 0,
                   mInit: Int = 0, range: Boolean = false, open var res: MutableMap<Int, Int>? = null, open var aSkills: Array<Ability>? = null,
                   aStates: Array<State>? = null, open var stRes: MutableMap<State, Int>? = null)
    : Role(id, name, mHp, mMp, mSp, mInit, range, aStates)