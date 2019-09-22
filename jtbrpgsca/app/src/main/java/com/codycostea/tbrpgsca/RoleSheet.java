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
import android.graphics.Bitmap;
import android.graphics.drawable.AnimationDrawable;
import android.graphics.drawable.BitmapDrawable;
import android.graphics.drawable.Drawable;
import android.os.Parcel;
import android.os.Parcelable;

import java.util.Locale;

public abstract class RoleSheet implements Parcelable {

    public static String hpText = "HP";
    public static String mpText = "MP";
    public static String spText = "RP";

	protected final static int FLAG_RANGE = 1;

	protected String name;
	protected int id, mHp, mMp, mSp, mInit, flags = 0;
	protected StateMask[] aStates;

	public static int GetSpriteDuration(final AnimationDrawable sprAnim) {
		int s = 0;
		for (int i = 0; i < sprAnim.getNumberOfFrames(); i++) {
			s += sprAnim.getDuration(i);
		}
		return s;
	}

	public static AnimationDrawable GetInvertedSprite(final AnimationDrawable sprAnim, final boolean firstFrameWait) {
		final AnimationDrawable animSpr = new AnimationDrawable();
		final int sprNr = sprAnim.getNumberOfFrames() - 1;
		animSpr.setOneShot(true);
		for (int i = sprNr; i > -1; i--) {
			animSpr.addFrame(sprAnim.getFrame(i), (firstFrameWait && i == sprNr) ? 261 : 87);
		}
		return animSpr;
	}

	public static AnimationDrawable GetBitmapSprite(final BitmapDrawable spriteBmp, final Context context, final Drawable firstFrame,
											  final boolean firstFrameWait, final Drawable lastFrame, final boolean addPlayback) {
		final Bitmap bmp = spriteBmp.getBitmap();
		final AnimationDrawable animSpr = new AnimationDrawable();
		animSpr.setOneShot(true);
		if (firstFrameWait && firstFrame != null) {
			animSpr.addFrame(firstFrame, 261);
		}
		int sprWidth = bmp.getWidth();
		final int sprHeight = bmp.getHeight();
		final int sprCount = bmp.getWidth() / sprHeight;
		sprWidth /= sprCount;
		final int lastSprDur = (addPlayback && sprCount < 7) ? 261 : 87;
		for (int i = 0; i < sprCount; i++) {
			animSpr.addFrame(new BitmapDrawable(Bitmap.createBitmap(bmp, i * sprWidth, 0, sprWidth, sprHeight)),
			(i < sprCount - 1) ? 87 : lastSprDur);
		}
		if (lastSprDur == 261) {
			for (int i = sprCount - 2; i > 0; i--) {
				animSpr.addFrame(animSpr.getFrame(i), 87);
			}
		}
		if (lastFrame != null) {
			animSpr.addFrame(lastFrame, 1);
		}
		//bmp.recycle();
		return animSpr;
	}

	protected RoleSheet(final Parcel in) {
		this.name = in.readString();
		this.id = in.readInt();
		this.mHp = in.readInt();
		this.mMp = in.readInt();
		this.mSp = in.readInt();
		this.mInit = in.readInt();
		this.flags = in.readInt();
		this.aStates = in.createTypedArray(StateMask.CREATOR);
	}

	@Override
	public void writeToParcel(final Parcel dest, final int flags) {
		dest.writeString(this.name);
		dest.writeInt(this.id);
		dest.writeInt(this.mHp);
		dest.writeInt(this.mMp);
		dest.writeInt(this.mSp);
		dest.writeInt(this.mInit);
		dest.writeInt(this.flags);
		dest.writeTypedArray(this.aStates, flags);
	}

	@Override
	public int describeContents() {
		return 0;
	}

	public static final Creator<RoleSheet> CREATOR = new Creator<RoleSheet>() {
		@Override
		public RoleSheet createFromParcel(Parcel in) {
			return null;
		}

		@Override
		public RoleSheet[] newArray(int size) {
			return new RoleSheet[size];
		}
	};

	public int getId() {
		return this.id;
	}

	public String getName() {
		return this.name;
	}

	public RoleSheet setName(final String value) {
		this.name = value;
		return this;
	}

	public int getMaxHpMod() {
		return this.mHp;
	}

	public RoleSheet setMaxHpMod(final int value) {
		this.mHp = value;
		return this;
	}

	public int getMaxMpMod() {
		return this.mMp;
	}

	public RoleSheet setMaxMpMod(final int value) {
		this.mMp = value;
		return this;
	}

	public int getMaxSpMod() {
		return this.mSp;
	}

	public RoleSheet setMaxSpMod(final int value) {
		this.mSp = value;
		return this;
	}

	public int getMaxInitMod() {
		return this.mInit;
	}

	public RoleSheet setMaxInitMod(final int value) {
		this.mInit = value;
		return this;
	}

	public boolean hasRange() {
		return (this.flags & FLAG_RANGE) == FLAG_RANGE;
	}

	public RoleSheet setRange(final boolean value) {
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

	public RoleSheet setAddedStates(final StateMask[] value) {
		this.aStates = value;
		return this;
	}

	public boolean equals(final Object object) {
		return object != null && object instanceof RoleSheet && ((RoleSheet)object).id == this.id;
	}

	public RoleSheet(final int id, final String name, final int hp, final int mp, final int sp,
                     final int mInit, final boolean range, final StateMask[] states) {
		this.id = id;
		this.name = name;
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
			s.append(String.format(Locale.US, "%d %s", -dmghp, RoleSheet.hpText));
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
			s.append(String.format(Locale.US, "%d %s", -dmgmp, RoleSheet.mpText));
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
			s.append(String.format(Locale.US, "%d %s", -dmgsp, RoleSheet.spText));
		}
		return s.toString();
	}

}