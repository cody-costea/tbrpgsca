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
package com.codycostea.tbrpgsca;

import android.os.Parcel;
import android.os.Parcelable;

public abstract class RolePlay implements Parcelable {

	protected final static int FLAG_RANGE = 1;

	protected String name, sprite;
	protected int id, mHp, mMp, mSp, mInit, flags = 0;
	protected StateMask[] aStates;

	protected RolePlay(final Parcel in) {
		name = in.readString();
		sprite = in.readString();
		id = in.readInt();
		mHp = in.readInt();
		mMp = in.readInt();
		mSp = in.readInt();
		mInit = in.readInt();
		flags = in.readInt();
		aStates = in.createTypedArray(StateMask.CREATOR);
	}

	@Override
	public void writeToParcel(final Parcel dest, final int flags) {
		dest.writeString(name);
		dest.writeString(sprite);
		dest.writeInt(id);
		dest.writeInt(mHp);
		dest.writeInt(mMp);
		dest.writeInt(mSp);
		dest.writeInt(mInit);
		dest.writeInt(flags);
		dest.writeTypedArray(aStates, flags);
	}

	@Override
	public int describeContents() {
		return 0;
	}

	public static final Creator<RolePlay> CREATOR = new Creator<RolePlay>() {
		@Override
		public RolePlay createFromParcel(Parcel in) {
			return null;
		}

		@Override
		public RolePlay[] newArray(int size) {
			return new RolePlay[size];
		}
	};

	public int getId() {
		return this.id;
	}

	public String getName() {
		return this.name;
	}

	public RolePlay setName(final String value) {
		this.name = value;
		return this;
	}

	public String getSprite() {
		return this.sprite;
	}

	public RolePlay setSprite(final String value) {
		this.sprite = value;
		return this;
	}

	public int getMaxHpMod() {
		return this.mHp;
	}

	public RolePlay setMaxHpMod(final int value) {
		this.mHp = value;
		return this;
	}

	public int getMaxMpMod() {
		return this.mMp;
	}

	public RolePlay setMaxMpMod(final int value) {
		this.mMp = value;
		return this;
	}

	public int getMaxSpMod() {
		return this.mSp;
	}

	public RolePlay setMaxSpMod(final int value) {
		this.mSp = value;
		return this;
	}

	public int getMaxInitMod() {
		return this.mInit;
	}

	public RolePlay setMaxInitMod(final int value) {
		this.mInit = value;
		return this;
	}

	public boolean hasRange() {
		return (this.flags & FLAG_RANGE) == FLAG_RANGE;
	}

	public RolePlay setRange(final boolean value) {
		if (value) {
			this.flags |= FLAG_RANGE;
		} else {
			int flags = this.flags;
			if ((flags & FLAG_RANGE) == FLAG_RANGE) {
				flags -= FLAG_RANGE;
				this.flags = flags;
			}
		}
		return this;
	}

	public StateMask[] getAddedStates() {
		return this.aStates;
	}

	public RolePlay setAddedStates(final StateMask[] value) {
		this.aStates = value;
		return this;
	}

	public boolean equals(final Object object) {
		return object != null && object instanceof RolePlay && ((RolePlay)object).id == this.id;
	}

	public RolePlay(final int id, final String name, final String sprite, final int hp, final int mp,
					final int sp, final int mInit, final boolean range, final StateMask[] states) {
		this.id = id;
		this.name = name;
		this.sprite = sprite;
		this.mHp = hp;
		this.mMp = mp;
		this.mSp = sp;
		this.mInit = mInit;
		if (range) {
			this.flags |= FLAG_RANGE;
		}
		this.aStates = states;
	}

	static String getDmgText(final int dmghp, final int dmgmp, final int dmgsp) {
		final StringBuffer s = new StringBuffer();
		boolean c = false;
		if (dmghp != 0) {
			s.append(" ");
			if (dmghp < 0) {
				s.append("+");
			}
			s.append(String.valueOf(-dmghp)).append(" HP");
			c = true;
		}
		if (dmgmp != 0) {
			if (c) {
				s.append(",");
			}
			s.append(" ");
			if (dmgmp < 0) {
				s.append("+");
			}
			s.append(String.valueOf(-dmgmp)).append(" MP");
			c = true;
		}
		if (dmgsp != 0) {
			if (c) {
				s.append(",");
			}
			s.append(" ");
			if (dmgsp < 0) {
				s.append("+");
			}
			s.append(String.valueOf(-dmgsp)).append(" RP");
		}
		return s.toString();
	}

}