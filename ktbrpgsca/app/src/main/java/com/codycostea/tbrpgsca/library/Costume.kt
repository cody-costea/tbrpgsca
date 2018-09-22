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

open class Costume(val id : Int, open var name : String, open var mHp : Int = 30, open var mMp : Int = 10, open var mSp : Int = 10,
                   open var atk : Int = 7, open var def: Int = 7, open var spi: Int = 7, open var wis : Int = 7, open var agi : Int = 7,
                   open var mActions : Int = 0, open var mInit:Int = 0, open var range : Boolean = false, open var res : MutableMap<Int, Int>? = null,
                   open var skills : Array<Ability>? = null, open var states : Array<State>? = null, open var stRes : MutableMap<State, Int>? = null) {

    companion object {
        var hpText = "HP"
        var mpText = "MP"
        var spText = "RP"

        fun getDmgText(dmghp: Int, dmgmp: Int, dmgsp: Int): String {
            var s = ""
            var c = false
            if (dmghp != 0) {
                s += " "
                if (dmghp < 0) {
                    s += "+"
                }
                s += "${-dmghp} $hpText"
                c = true
            }
            if (dmgmp != 0) {
                if (c) {
                    s += ","
                }
                s += " "
                if (dmgmp < 0) {
                    s += "+"
                }
                s += "${-dmgmp} $mpText"
                c = true
            }
            if (dmgsp != 0) {
                if (c) {
                    s += ","
                }
                s += " "
                if (dmgsp < 0) {
                    s += "+"
                }
                s += "${-dmgsp} $spText"
            }
            return s
        }
    }
}