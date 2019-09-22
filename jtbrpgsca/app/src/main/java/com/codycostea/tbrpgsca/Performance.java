/*
Copyright (C) AD 2013-2019 Claudiu-Stefan Costea

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
package com.codycostea.tbrpgsca;

import android.content.Context;
import android.graphics.drawable.AnimationDrawable;
import android.graphics.drawable.BitmapDrawable;
import android.graphics.drawable.Drawable;
import android.media.MediaPlayer;
import android.os.Parcel;

import java.util.Enumeration;
import java.util.Hashtable;
import java.util.Locale;
import java.util.Random;

public final class Performance extends RoleSheet {

	public static String reflectedTxt = ", reflected by %s";
	public static String suffersTxt = ", %s suffers";
	public static String stolenTxt = ", obtaining %s from %s";
	public static String missesTxt = ", but misses";

	protected final static int FLAG_STEAL = 2;
	protected final static int FLAG_ABSORB = 4;
	protected final static int FLAG_RESTORE = 8;

	public final static int TRG_ONE = 0;
	public final static int TRG_SELF = -1;
	public final static int TRG_ENEMY = 1;
	public final static int TRG_PARTY = -2;
	public final static int TRG_ALL = 2;

	public static final int DMG_TYPE_ATK = 1;
	public static final int DMG_TYPE_DEF = 2;
	public static final int DMG_TYPE_SPI = 4;
	public static final int DMG_TYPE_WIS = 8;
	public static final int DMG_TYPE_AGI = 16;

	private MediaPlayer _sndPlayer;
	private AnimationDrawable _sprAnim;
	private Context _context;

	protected int sprId, sndId, lvRq, hpC, mpC, spC, mQty, rQty, dmgType, trg, spriteDur;
	protected StateMask[] rStates;
	protected Integer elm;

	protected Performance(final Parcel in) {
		super(in);
		this.sprId = in.readInt();
		this.sndId = in.readInt();
		this.lvRq = in.readInt();
		this.hpC = in.readInt();
		this.mpC = in.readInt();
		this.spC = in.readInt();
		this.mQty = in.readInt();
		this.rQty = in.readInt();
		this.dmgType = in.readInt();
		this.trg = in.readInt();
		this.rStates = in.createTypedArray(StateMask.CREATOR);
		if (in.readByte() == 0) {
			this.elm = null;
		} else {
			this.elm = in.readInt();
		}
	}

	@Override
	public void writeToParcel(final Parcel dest, final int flags) {
		super.writeToParcel(dest, flags);
		dest.writeInt(this.sprId);
		dest.writeInt(this.sndId);
		dest.writeInt(this.lvRq);
		dest.writeInt(this.hpC);
		dest.writeInt(this.mpC);
		dest.writeInt(this.spC);
		dest.writeInt(this.mQty);
		dest.writeInt(this.rQty);
		dest.writeInt(this.dmgType);
		dest.writeInt(this.trg);
		dest.writeTypedArray(this.rStates, flags);
		final Integer elm = this.elm;
		if (elm == null) {
			dest.writeByte((byte) 0);
		} else {
			dest.writeByte((byte) 1);
			dest.writeInt(elm);
		}
	}

	@Override
	public int describeContents() {
		return 0;
	}

	public static final Creator<Performance> CREATOR = new Creator<Performance>() {
		@Override
		public Performance createFromParcel(Parcel in) {
			return new Performance(in);
		}

		@Override
		public Performance[] newArray(int size) {
			return new Performance[size];
		}
	};

	public AnimationDrawable getSpriteAnimation(final Context context) {
		final int sprId = this.sprId;
		if (sprId < 1) return null;
		AnimationDrawable sprAnim = this._sprAnim;
		if (this._context != context) {
			sprAnim = null;
			MediaPlayer sndPlayer = this._sndPlayer;
			if (sndPlayer != null) {
				sndPlayer.release();
				this._sndPlayer = null;
			}
		}
		if (sprAnim == null) {
			Drawable drawable = context.getResources().getDrawable(sprId);
			if (drawable instanceof AnimationDrawable) {
				sprAnim = (AnimationDrawable) drawable;
			} else if (drawable instanceof BitmapDrawable) {
				sprAnim = RoleSheet.GetBitmapSprite((BitmapDrawable)drawable, context, null,
						false, null, false);
			}
			if (sprAnim == null) {
				this.spriteDur = 0;
			} else {
				this.spriteDur = RoleSheet.GetSpriteDuration(sprAnim);
			}
			this._sprAnim = sprAnim;
		}
		return sprAnim;
	}

	public int playSound(final Context context) {
		final int sndId = this.sndId;
		if (sndId < 1) return 0;
		MediaPlayer sndPlayer = this._sndPlayer;
		if (this._context != context) {
			this._sprAnim = null;
			this.spriteDur = 0;
			if (sndPlayer != null) {
				sndPlayer.release();
				sndPlayer = null;
			}
		}
		if (sndPlayer == null) {
			sndPlayer = MediaPlayer.create(context, sndId);
			if (sndPlayer == null) return 0;
			this._context = context;
			sndPlayer.setLooping(false);
			this._sndPlayer = sndPlayer;
		}
		sndPlayer.start();
		return sndPlayer.getDuration();
	}

	public int getSoundId() {
		return this.sndId;
	}

	public Performance setSoundId(final int value) {
		this.sndId = value;
		final MediaPlayer sndPlayer = this._sndPlayer;
		if (sndPlayer != null) {
			sndPlayer.release();
			this._sndPlayer = null;
		}
		return this;
	}

	public int getSpriteId() {
		return this.sprId;
	}

	public Performance setSpriteId(final int value) {
		this.sprId = value;
		this._sprAnim = null;
		return this;
	}

	public int getRequiredLevel() {
		return this.lvRq;
	}

	public Performance setRequiredLevel(final int value) {
		this.lvRq = value;
		return this;
	}

	public int getHpCost() {
		return this.hpC;
	}

	public Performance setHpCost(final int value) {
		this.hpC = value;
		return this;
	}

	public int getMpCost() {
		return this.mpC;
	}

	public Performance setMpCost(final int value) {
		this.mpC = value;
		return this;
	}

	public int getSpCost() {
		return this.spC;
	}

	public Performance setSpCost(final int value) {
		this.spC = value;
		return this;
	}

	public int getMaxUsesQty() {
		return this.mQty;
	}

	public Performance setMaxUsesQty(final int value) {
		this.mQty = value;
		return this;
	}

	public int getUsesQtyRegen() {
		return this.rQty;
	}

	public Performance setUsesQtyRegen(final int value) {
		this.rQty = value;
		return this;
	}

	public int getDamageType() {
		return this.dmgType;
	}

	public Performance setDamageType(final int value) {
		this.dmgType = value;
		return this;
	}

	public int getTargetRange() {
		return this.trg;
	}

	public Performance setTargetRange(final int value) {
		this.trg = value;
		return this;
	}

	public Integer getElement() {
		return this.elm;
	}

	public Performance setElement(final Integer value) {
		this.elm = value;
		return this;
	}

	public StateMask[] getRemovedStates() {
		return this.rStates;
	}

	public Performance setRemovedStates(final StateMask[] value) {
		this.rStates = value;
		return this;
	}

	public boolean isStealing() {
		return (this.flags & FLAG_STEAL) == FLAG_STEAL;
	}

	public Performance setStealing(final boolean value) {
		if (value) {
			this.flags |= FLAG_STEAL;
		} else {
			int flags = this.flags;
			if ((flags & FLAG_STEAL) == FLAG_STEAL) {
				flags -= FLAG_STEAL;
				this.flags = flags;
			}
		}
		return this;
	}

	public boolean isAbsorbing() {
		return (this.flags & FLAG_ABSORB) == FLAG_ABSORB;
	}

	public Performance setAbsorbing(final boolean value) {
		if (value) {
			this.flags |= FLAG_ABSORB;
		} else {
			int flags = this.flags;
			if ((flags & FLAG_ABSORB) == FLAG_ABSORB) {
				flags -= FLAG_ABSORB;
				this.flags = flags;
			}
		}
		return this;
	}

	public boolean isRestoring() {
		return (this.flags & FLAG_RESTORE) == FLAG_RESTORE;
	}

	public Performance setRestoring(final boolean value) {
		if (value) {
			this.flags |= FLAG_RESTORE;
		} else {
			int flags = this.flags;
			if ((flags & FLAG_RESTORE) == FLAG_RESTORE) {
				flags -= FLAG_RESTORE;
				this.flags = flags;
			}
		}
		return this;
	}

	public String execute(final Interpreter user, Interpreter target, final boolean applyCosts) {
		final StringBuilder s = new StringBuilder();
		final Random rnd = new Random();
		final int dmgType = this.dmgType;
		int dmg = rnd.nextInt(4);
		if (target.isReflecting() && ((dmgType & DMG_TYPE_WIS) == DMG_TYPE_WIS)) {
			s.append(String.format(Locale.US, Performance.reflectedTxt, target.name));
			target = user;
		}
		Object o;
		final boolean ko = target._hp < 1;
		final Hashtable trgResMap = target.res;
		int res = trgResMap == null ? 3 : ((o = trgResMap.get(this.elm)) == null ? 3 : ((Integer)o).intValue());
		if (res > 6) {
			res = -1;
		}
		int i = 0;
		int def = 0;
		int canMiss = 0;
		if ((dmgType & DMG_TYPE_ATK) == DMG_TYPE_ATK) {
			canMiss = 2;
			dmg += user.atk;
			def += target.def;
			i++;
		}
		if ((dmgType & DMG_TYPE_DEF) == DMG_TYPE_DEF) {
			dmg += user.def;
			def += target.def;
			i++;
		}
		if ((dmgType & DMG_TYPE_SPI) == DMG_TYPE_SPI) {
			dmg += user.spi;
			def += target.wis;
			i++;
		}
		if ((dmgType & DMG_TYPE_WIS) == DMG_TYPE_WIS) {
			dmg += user.wis;
			def += target.spi;
			i++;
		}
		if ((dmgType & DMG_TYPE_AGI) == DMG_TYPE_AGI) {
			canMiss = -canMiss + 4;
			dmg += user.agi;
			def += target.agi;
			i++;
		}
		dmg = i == 0 ? 0 : (this.mInit + (dmg / i)) / (def / i * res + 1);
		if (canMiss == 0 || ((rnd.nextInt(13) + user.agi / canMiss)) > 2 + target.agi / 4) {
			int hpDmg = this.mHp;
			hpDmg = hpDmg == 0 ? 0 : ((hpDmg < 0 ? -1 : 1) * dmg + hpDmg);
			int mpDmg = this.mMp;
			mpDmg = mpDmg == 0 ? 0 : ((mpDmg < 0 ? -1 : 1) * dmg + mpDmg);
			int spDmg = this.mSp;
			spDmg = spDmg == 0 ? 0 : ((spDmg < 0 ? -1 : 1) * dmg + spDmg);
			if (res < 0) {
				hpDmg = -hpDmg;
				mpDmg = -mpDmg;
				spDmg = -spDmg;
			}
			target.setCurrentHp(target._hp - hpDmg);
			target.setCurrentMp(target._mp - mpDmg);
			target.setCurrentSp(target._sp - spDmg);
			if (this.isAbsorbing()) {
				user.setCurrentHp(user._hp + hpDmg / 2);
				user.setCurrentMp(user._mp + mpDmg / 2);
				user.setCurrentSp(user._sp + spDmg / 2);
			}
			if (hpDmg != 0 || mpDmg != 0 || spDmg != 0) {
				s.append(String.format(Locale.US, Performance.suffersTxt, target.name));
				s.append(RoleSheet.getDmgText(hpDmg, mpDmg, spDmg));
			}
			String r;
			final StateMask[] aStates = this.aStates;
			if (aStates != null) {
				for (int j = 0; j < aStates.length; j++) {
					r = aStates[j].inflict(target, false, false);
					if (r.length() > 0) {
						s.append(r);
					}
				}
			}
			final Hashtable trgStateMap = target.stateDur;
			if (trgStateMap != null) {
				final StateMask[] rStates = this.rStates;
				if (rStates != null) {
					for (int j = 0; j < rStates.length; j++) {
						final Enumeration trgStates = trgStateMap.keys();
						while (trgStates.hasMoreElements()) {
							final StateMask state = (StateMask)trgStates.nextElement();
							if (rStates[j].equals(state)) {
								state.remove(target, false, false);
							}
						}
					}
				}
			}
			final Hashtable trgItemMap = target._items;
			if (this.isStealing() && trgItemMap != null && trgItemMap != user._items && trgItemMap.size() > 0
					&& (rnd.nextInt(12) + user.agi / 4) > 4 + target.agi / 3) {
				final int rndItem = rnd.nextInt(trgItemMap.size() - 1);
				final Enumeration itemsEnum = trgItemMap.keys();
				Performance stolen = null;
				for (int j = 0; j <= rndItem && itemsEnum.hasMoreElements(); j++) {
					if (j == rndItem) {
						stolen = (Performance)itemsEnum.nextElement();
					} else {
						itemsEnum.nextElement();
					}
				}
				int trgItemQty;
				final Object itemQtyObj = trgItemMap.get(stolen);
				if (itemQtyObj != null && ((trgItemQty = ((Integer)itemQtyObj).intValue()) > 0)) {
					Hashtable usrItems = user._items;
					if (usrItems == null) {
						usrItems = new Hashtable();
						user._items = usrItems;
					}
					final Object usrItem;
					usrItems.put(stolen, new Integer((usrItem = usrItems.get(stolen)) == null ? 0 : ((Integer)usrItem).intValue() + 1));
					trgItemQty--;
					if (trgItemQty == 0) {
						trgItemMap.remove(stolen);
					} else {
						trgItemMap.put(stolen, new Integer(trgItemQty));
					}
					s.append(String.format(Locale.US, Performance.stolenTxt, stolen.name, target.name));
				}
			}
			s.append(target.checkStatus());
		}
		else {
			s.append(Performance.missesTxt);
		}
		if (applyCosts) {
			user._hp -= this.hpC;
			user._mp -= this.mpC;
			user._sp -= this.spC;
			if (this.mQty > 0) {
				Hashtable usrSkillsQty = user.skillsQty;
				if (usrSkillsQty == null) {
					usrSkillsQty = new Hashtable();
					user.skillsQty = usrSkillsQty;
				}
				final Object usrItemQty;
				usrSkillsQty.put(this, new Integer(((usrItemQty = usrSkillsQty.get(this)) == null ? this.mQty : ((Integer)usrItemQty).intValue()) - 1));
			}
		}
		if (ko && target._hp > 0) {
			target.applyStates(false);
		}
		return s.append(user.checkStatus()).toString();
	}

	public void replenish(final Interpreter user) {
		if (this.mQty > 0) {
			Hashtable usrSkills = user.skillsQty;
			if (usrSkills == null) {
				usrSkills = new Hashtable();
				user.skillsQty = usrSkills;
			}
			usrSkills.put(this, new Integer(this.mQty));
		}
	}

	public boolean canPerform(final Interpreter actor) {
		final Object itemQty;
		final Hashtable skillsQty = actor.skillsQty;
		return this.mpC <= actor._mp && this.hpC < actor._hp && this.spC <= actor._sp && actor._lv >= this.lvRq
				&& (skillsQty == null || ((itemQty = skillsQty.get(this)) == null ? 1 : ((Integer)itemQty).intValue()) > 0);
	}

	public Performance(final int id, final String name, final int sprId, final int sndId, final boolean steal,
					   final boolean range, final int lvRq, final int hpC, final int mpC, final int spC, final int dmgType,
					   final int atkI, final int hpDmg, final int mpDmg, final int spDmg, final int trg, final Integer elm,
					   final int mQty, final int rQty, final boolean absorb, final boolean restoreKO, final StateMask[] aStates,
					   final StateMask[] rStates) {
		super(id, name, hpDmg, mpDmg, spDmg, atkI, range, aStates);
		this.sprId = sprId;
		this.sndId = sndId;
		if (steal) {
			this.flags |= FLAG_STEAL;
		}
		this.lvRq = lvRq;
		this.hpC = hpC;
		this.mpC = mpC;
		this.spC = spC;
		this.dmgType = dmgType;
		this.trg = trg;
		this.elm = elm;
		this.mQty = mQty;
		this.rQty = rQty;
		if (absorb) {
			this.flags |= FLAG_ABSORB;
		}
		if (restoreKO) {
			this.flags |= FLAG_RESTORE;
		}
		this.rStates = rStates;
	}

}