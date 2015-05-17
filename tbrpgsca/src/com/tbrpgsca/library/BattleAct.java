/*
Copyright (C) 2015 Claudiu-Stefan Costea

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
package com.tbrpgsca.library;

import java.util.ArrayList;

import android.os.Bundle;
import android.app.Activity;
import android.app.AlertDialog;
import android.content.DialogInterface;
import android.content.Intent;
import android.graphics.drawable.AnimationDrawable;
import android.text.method.ScrollingMovementMethod;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.Spinner;
import android.widget.TextView;

public class BattleAct extends Activity
{

	Ability[] Skill = AppData.Skill;
    Ability[] Item = AppData.Item;
    Actor[] Player = {null,null,null,null,null,null,null,null};
    int target,difference,current=Player.length-1,result=0;
    int waitTime[]={0,0,0};
    
    boolean surprised=false;
    
    Spinner targetBox, skillBox,itemBox;
    ArrayAdapter<String> targetList,skillList,itemList;
    TextView skillCost, updText;
    Button Act,Use,Auto,Escape;
    ImageView imgActor[] = new ImageView[8];
    
    ArrayList<Integer> skillId=new ArrayList<Integer>();
    ArrayList<Integer> itemId=new ArrayList<Integer>();
    
	@Override
	protected void onCreate(Bundle savedInstanceState)
	{
		super.onCreate(savedInstanceState);
		this.setContentView(R.layout.activity_battle);
		this.targetBox=(Spinner) findViewById(R.id.TargetBox);
		this.skillBox=(Spinner) findViewById(R.id.SkillBox);
		this.itemBox=(Spinner) findViewById(R.id.ItemBox);
		this.skillCost=(TextView) findViewById(R.id.SkillCost);
		this.updText=(TextView) findViewById(R.id.ItemCost);
		this.Act=(Button) findViewById(R.id.ActBt);
		this.Use=(Button) findViewById(R.id.UseBt);
		this.Escape=(Button) findViewById(R.id.RunBt);
		this.Auto=(Button) findViewById(R.id.AutoBt);
		this.skillList=new ArrayAdapter<String>(this,android.R.layout.simple_spinner_item);
		this.itemList=new ArrayAdapter<String>(this,android.R.layout.simple_spinner_item);
		this.targetList=new ArrayAdapter<String>(this,android.R.layout.simple_spinner_item);
		this.targetBox.setAdapter(targetList);
		this.skillBox.setAdapter(skillList);
		this.itemBox.setAdapter(itemList);
		this.Act.setOnClickListener(cAction);
		this.Use.setOnClickListener(cAction);
		this.Escape.setOnClickListener(cAction);
		this.Auto.setOnClickListener(cAction);
		this.updText.setMovementMethod(new ScrollingMovementMethod());
		Bundle extra = this.getIntent().getExtras();	    
		this.Escape.setEnabled((extra!=null&&extra.containsKey("Escape"))?extra.getBoolean("Escape"):true);
	    int e=(extra!=null&&extra.containsKey("Enemy"))?extra.getInt("Enemy"):0;
	    this.beginBattle(AppData.party,AppData.enemy[e],(extra!=null&&extra.containsKey("Surprise"))?extra.getInt("Surprise"):0);
	}
	
    public void beginBattle(int party[],int enemy[], int surprise)
    {
    	this.surprised=(surprise<0);
    	if (this.surprised)
    	{
    		this.imgActor[0]=(ImageView) findViewById(R.id.ImgEnemy3);
    		this.imgActor[1]=(ImageView) findViewById(R.id.ImgEnemy4);
    		this.imgActor[2]=(ImageView) findViewById(R.id.ImgEnemy1);
    		this.imgActor[3]=(ImageView) findViewById(R.id.ImgEnemy2);
    		this.imgActor[4]=(ImageView) findViewById(R.id.ImgPlayer3);
    		this.imgActor[5]=(ImageView) findViewById(R.id.ImgPlayer4);
    		this.imgActor[6]=(ImageView) findViewById(R.id.ImgPlayer1);
    		this.imgActor[7]=(ImageView) findViewById(R.id.ImgPlayer2);
    	} else
    	{
    		this.imgActor[0]=(ImageView) findViewById(R.id.ImgPlayer1);
    		this.imgActor[1]=(ImageView) findViewById(R.id.ImgPlayer2);
    		this.imgActor[2]=(ImageView) findViewById(R.id.ImgPlayer3);
    		this.imgActor[3]=(ImageView) findViewById(R.id.ImgPlayer4);
    		this.imgActor[4]=(ImageView) findViewById(R.id.ImgEnemy1);
    		this.imgActor[5]=(ImageView) findViewById(R.id.ImgEnemy2);
    		this.imgActor[6]=(ImageView) findViewById(R.id.ImgEnemy3);
    		this.imgActor[7]=(ImageView) findViewById(R.id.ImgEnemy4);
    	}
    	this.difference=0;
    	int j=0;
    	for (int i=0;i<party.length;i++)
    	{
    		this.Player[j]=AppData.Player[party[i]];
    		if (this.Player[j].maxhp<1) this.difference++;
    		j++;
    	}
    	for (int k=0;k<enemy.length;k++)
    		{this.Player[j]=new Actor();this.Player[j].copy(AppData.Player[enemy[k]]);this.Player[j].auto=2;j++;}    	
        for (int i=0;i<this.imgActor.length;i++) {this.imgActor[i].setOnClickListener(this.cAction);this.imgActor[i].setTag(0);this.playSpr(i,this.Player[i].hp>0?0:-1);}
        if (surprise<0) {for(int i=0;i<(this.Player.length/2)-this.difference;i++)this.Player[i].active=false;this.endTurn();}
        	else if (surprise>0) {for(int i=(this.Player.length/2);i<this.Player.length;i++)this.Player[i].active=false;this.endTurn();}
        		else this.endTurn();
        this.refreshTargetBox();
        this.targetBox.setSelection((this.Player.length/2)-this.difference);
        this.refreshSkillBox();
        this.refreshItemBox();
    }
    
    private void refreshTargetBox()
    {
    	this.targetList.clear();
    	String s;
        for (int i=0;i<this.Player.length;i++)
        {
            if (this.Player[i].maxhp>0){
            	s=this.Player[i].name+" (HP: "+this.Player[i].hp;
            	if (i<(this.Player.length/2)-this.difference) s+=", MP: "+this.Player[i].mp+", SP: "+this.Player[i].sp;
            	s+=")";
            	this.targetList.add(s);
            }
        }
        this.targetList.notifyDataSetChanged();
    }
    private void refreshSkillBox()
    {
        skillList.clear();skillId.clear();String trg;
        for (int i=0;i<Player[current].skill.size();i++)
            if (!(Player[current].skill.get(i).getLvRq()>Player[current].level)&&!(Player[current].skill.get(i).mpc>Player[current].mp)&&
                    !(Player[current].skill.get(i).spc>Player[current].sp)&&!(Player[current].skill.get(i).hpc>=Player[current].hp)){
            	trg=(Player[current].skill.get(i).trg>0)?"All":"Self"; if (Player[current].skill.get(i).trg==0) trg="One"; skillId.add(i);
                        skillList.add(Player[current].skill.get(i).name+" (Rq:" + Player[current].skill.get(i).hpc + "HP," + Player[current].skill.get(i).mpc +
                        		"MP," + Player[current].skill.get(i).spc + "SP;" + "Trg:"+trg+")");}
        skillList.notifyDataSetChanged();
        if (skillBox.getSelectedItemPosition()>1) skillBox.setSelection(0);
    }
    
    private void refreshItemBox()
    {
        itemList.clear();itemId.clear();String trg;
        for (int i=0;i<Item.length;i++)
            if (Item[i].qty>0)
            {
            	trg=(Item[i].trg>0)?"All":"Self"; if (Item[i].trg==0) trg="One"; itemId.add(i);
                itemList.add(Item[i].name+" (Qty:"+Item[i].qty+";" + "Trg:"+trg+")");
        }
        if (itemList.getCount()==0) { Use.setEnabled(false); itemBox.setEnabled(false);}
        	else { Use.setEnabled(true); itemBox.setEnabled(true); }
        itemList.notifyDataSetChanged();
    }    
    
    private void executeSkill()
    {    	
    	executeSkill(Player[current].skill.get(skillId.get(skillBox.getSelectedItemPosition())));
    }
    
    private void executeSkill(Ability ability)
    {
    	if (target==1&&Player[3].hp>0&&(Player[0].hp>0||Player[2].hp>0)&&current>(Player.length/2)-1&&difference==0&&!ability.range) target=3;
    	if (target==2&&Player[0].hp>0&&(Player[1].hp>0||Player[3].hp>0)&&current>(Player.length/2)-1&&difference<2&&!ability.range) target=0;
    	if (target==4&&Player[6].hp>0&&(Player[5].hp>0||Player[7].hp>0)&&!ability.range) target=6;
        if (target==7&&(Player[6].hp>0||Player[4].hp>0)&&Player[5].hp>0&&!ability.range) target=5;
        while (Player[target].hp<1&&!(ability.state[0]&&((ability.hpdmg<0&&Player[target].mres[ability.element]<7)||
        		(ability.hpdmg>0&&Player[target].mres[ability.element]==7))))
        {
        	if (ability.hpdmg<0) target--; else target++;
        	if (target<0) target=(Player.length/2)-1-difference;
            if (target>Player.length-difference) target=(Player.length/2)-difference;
        }
        int a=target,b=target;
        if (ability.trg>1) {a=0;b=6;}
        if (current>(Player.length/2)-1-difference&&ability.trg<-1) {a=(Player.length/2);b=Player.length-1;}
        if (current<(Player.length/2)-difference&&ability.trg<-1) {a=0;b=(Player.length/2)-1;}
        if (target>(Player.length/2)-1-difference&&ability.trg==1) {a=(Player.length/2);b=Player.length-1;}
        if (target<(Player.length/2)-difference&&ability.trg==1) {a=0;b=(Player.length/2)-1;}        
        if (ability.trg==-1) {a=current;b=current;}
        this.updText.append("\n"+Player[current].name + " performs " + ability.name);
        this.Player[current].hp -= ability.hpc;
        this.Player[current].mp -= ability.mpc;
        this.Player[current].sp -= ability.spc;
        if (ability.qty>0) ability.qty--;
        int trg;boolean act=true;
    	for (int i=a;i<=b;i++) if (Player[i].hp>0||ability.state[0])
    	{        	
        	if (Player[i].reflect&&ability.dmgtype==2&&i!=current)
        	{
        		updText.append(", which is reflected");
        		trg=current;
        	} else trg=i;
        	if (act) {playSpr(current,3);act=false;}
        	updText.append(ability.execute(Player[current], Player[trg]));
        	boolean ko=Player[trg].hp<1;
        	if (trg!=current) playSpr(trg,ko?2:1);        	
        }
    	this.waitTime[0]=this.waitTime[1]>this.waitTime[2]?this.waitTime[1]:this.waitTime[2];
        if (this.Player[current].hp<1)
        	this.playSpr(current,2);
    	if (this.current<(Player.length/2)-difference) {Player[current].exp++; Player[current].levelUp();}
    	this.updText.append(".");
    }
    
    private void useItem()
    {
    	this.executeSkill(Item[itemId.get(itemBox.getSelectedItemPosition())]);
    }
    
    private void executeAI()
    {
    	this.setAItarget(getAIskill(checkAIheal()));
    }
    
    private boolean checkAIheal()
    {
    	boolean nHeal=false;
    	int a = (current>(Player.length/2)-1-difference) ? (Player.length/2)-difference : 0;
    	int b = (current<(Player.length/2)-difference) ? (Player.length/2)-1-difference : (Player.length/2)-1-difference;
    	for (int i=a;i<=b;i++)
    		if (Player[i].hp<(Player[i].maxhp/3))
    		{
    			nHeal=true;
    			break;
    		}
    	if (nHeal)
    	{
    		nHeal=false;
    		for (int i=0;i<Player[current].skill.size();i++)
    		{
    			Ability s=Player[current].skill.get(i);
    			if (s.hpdmg<0&&s.mpc<=Player[current].mp&&s.hpc<=Player[current].hp&&
        				s.spc<=Player[current].sp&&Player[current].level>=s.getLvRq())
    			{
    				nHeal=true;break;
    			}
    		}
    	}
    	return nHeal;
    }
    
    private Ability getAIskill(boolean heal)
    {
    	Ability s=(heal) ? Skill[1]:Skill[0];
    	Ability a;
    	for (int i=0;i<Player[current].skill.size();i++)
    	{
    		a=Player[current].skill.get(i);
    		if (a.mpc<=Player[current].mp&&a.hpc<Player[current].hp&&
    				a.spc<=Player[current].sp&&Player[current].level>=a.getLvRq())
    			if (heal) {if (a.hpdmg<s.hpdmg&&a.hpdmg<0) s=a;}
    			else if (a.hpdmg>s.hpdmg) s=a;
    	}
    	return s;
    }
    
    private void setAItarget(Ability ability)
    {
    	int a;
    	int b;
    	if (((this.current>(this.Player.length/2)-1-this.difference)&&(ability.hpdmg>0))||((this.current<(this.Player.length/2)-this.difference)&&(ability.hpdmg<1))) {a=0;b=(this.Player.length/2)-1;}
    		else {a=(this.Player.length/2);b=this.Player.length-1;}
    	if (this.Player[this.current].auto<0) if (a==(this.Player.length/2)) {a=0;b=(this.Player.length/2)-1;} else {a=(this.Player.length/2);b=this.Player.length-1;}
    	this.target=a;
    	while ((this.Player[target].hp<1)&&(ability.hpdmg>1)&&this.target<b) this.target++;
    	
    	for (int i=this.target;i<=b;++i)
    		if ((this.Player[i].hp<Player[target].hp)&&(this.Player[i].hp>0))
    			this.target = i;
    	this.executeSkill(ability);
    }
    
    @SuppressWarnings("deprecation")
    private void endTurn()
    {
    	this.imgActor[this.current].setBackgroundDrawable(null);
    	this.Player[this.current].active=false;

    	boolean reset=true;
    	for (int i=0;i<this.Player.length;i++)
    	{
    		if (this.Player[i].active) {reset=false;break;}
    	}
    	if (reset)
    	{
        	for (int i=0;i<this.Player.length;i++)
            	if (this.Player[i].hp>0)
            	{
            		this.updText.append(this.Player[i].applyState(true));
            		if (this.Player[i].hp<1) playSpr(i,2);
            	}
        }
    	for (this.current=0;current<this.Player.length;this.current++)
    	{
    		if (this.Player[this.current].active) break;
    	}
    	if (this.current<6)
    		for (int i=this.current+1;i<this.Player.length;i++)
    			if ((this.Player[current].agi<this.Player[i].agi)&&this.Player[i].active)
    				this.current=i;
    		
    	if (!this.checkEnd())
    	{
    		if (this.Player[current].active)
    		{	
    			if (this.Player[current].auto==0)
    			{
    				this.setCurrent();
    				this.refreshItemBox();
    			}
    			this.Player[current].applyState(false);
	    		if (this.Player[this.current].auto!=0&&this.Player[this.current].hp>0)
	    		{
	    			this.executeAI();this.endTurn();
	    		}
	    		this.refreshTargetBox();
        	} else this.endTurn();
    	}
    }
    
    private void setCurrent()
    {
    	this.skillCost.setText(this.Player[this.current].name+": "+this.Player[this.current].hp+"/"+this.Player[this.current].maxhp+" HP, "
				+this.Player[this.current].mp+"/"+Player[this.current].maxmp+" MP, "
				+this.Player[this.current].sp+"/"+Player[this.current].maxsp+" SP");
    	this.imgActor[this.current].setBackgroundResource(R.drawable.current);
    	this.refreshSkillBox();
    }
    
    private boolean checkEnd()
    {
    	boolean b=false;
    	if ((this.Player[0].hp<1)&&(this.Player[1].hp<1)&&(this.Player[2].hp<1)&&(this.Player[3].hp<1))
    		{this.endBt(-1);b=true;} else
        if ((this.Player[4].hp<1)&&(this.Player[5].hp<1)&&(this.Player[6].hp<1)&&(this.Player[7].hp<1))
        	{this.endBt(1);b=true;}
    	return b;
    }
    
    private void endBt(int r)
    {
    	String t="Defeat"; String s="The party has fled!";
    	if (r>0) {t="Victory"; s="The party has won the battle!";}
    	if (r<0) s="The party has been defeated!";
    	this.result=r;
    	this.endingMsg(t,s);
    }
    
    private void endingMsg(String t,String s)
    {
    	AlertDialog.Builder msg = new AlertDialog.Builder(this);
    	msg.setCancelable(false);
    	msg.setMessage(s);
    	msg.setTitle(t);
    	msg.setPositiveButton("Exit", new DialogInterface.OnClickListener()
    	{			
			@Override
			public void onClick(DialogInterface arg0, int arg1)
			{
				Intent outcome = new Intent();
				outcome.putExtra("Outcome", result);
				setResult(RESULT_OK, outcome);
				finish();
			}
		});
    	AlertDialog dialog = msg.create();
    	dialog.show();
    }
    
    private void playSpr(final int c, final int s)
    {
    	if (Player[c].getSprName()!=""){
    		final String pos=((c>(Player.length/2)-1&&!surprised)||(c<(Player.length/2)&&surprised))?"r":"l";    		
    		final AnimationDrawable sprAnim=Player[c].getBtSprite(s, pos, waitTime, this);    		
    		imgActor[c].postDelayed(new Runnable()
    		{
				@Override
				public void run()
				{
					if (s==2||s<0||Player[c].hp>0||!imgActor[c].getTag().equals(2))
					{
						sprAnim.stop();
						sprAnim.setOneShot(true);				    		
						imgActor[c].setImageDrawable(sprAnim);
						sprAnim.start();imgActor[c].setTag(s);				   		
					}
				}					
    		}, waitTime[0]);
    	} 	 
    }
    
    private boolean imgClick(int i)
    {
    	boolean a=false;    	
    	int s=(i>(Player.length/2)-1)?i-difference:i;
    	if (target==i)
    	{
    		executeSkill(getAIskill((i>Player.length/2-1&&current<Player.length/2)||(i<Player.length/2&&current>Player.length/2-1)?false:true));
			a=true;
		}
		else			
			if (Player[i].maxhp>0) targetBox.setSelection(s);
    	return a;
    }

    private OnClickListener cAction = new OnClickListener()
    {
    	@Override
    	public void onClick(final View v)
    	{
    		waitTime[0]=0;
    		boolean act=true;
    		target = targetBox.getSelectedItemPosition();
    		if ((target>2&&difference>0)||(target>0&&difference>2)||(target>1&&difference>1)) target+=difference;
    		switch (v.getId())
    		{
    			case R.id.ActBt:             
    				executeSkill();
    			break;
    			case R.id.UseBt:
    				useItem();
    			break;
    			case R.id.AutoBt:             
    				executeAI();
    			break;
    			case R.id.RunBt:             
    				endBt(0);
    			break;
    			case R.id.ImgPlayer1:             
    				act=(surprised)?imgClick(6):imgClick(0);
    			break;
    			case R.id.ImgPlayer2:             
    				act=(surprised)?imgClick(7):imgClick(1);
    			break;
    			case R.id.ImgPlayer3:             
    				act=(surprised)?imgClick(4):imgClick(2);
    			break;
    			case R.id.ImgPlayer4:             
    				act=(surprised)?imgClick(5):imgClick(3);
    			break;
    			case R.id.ImgEnemy1:             
    				act=(surprised)?imgClick(2):imgClick(4);
    			break;
    			case R.id.ImgEnemy2:             
    				act=(surprised)?imgClick(3):imgClick(5);
    			break;
    			case R.id.ImgEnemy3:
    				act=(surprised)?imgClick(0):imgClick(6);
    			break;
    			case R.id.ImgEnemy4:
    				act=(surprised)?imgClick(1):imgClick(7);
    			break;
    		}
    		if (act) endTurn();
    	}
    };
        
    @Override
    public void onBackPressed()
    {
    	endBt(-1);
    }
    
    @Override
    protected void onDestroy()
    {
    	for (Actor player:this.Player)
    		player.setSprites(this, false, null);
    	super.onDestroy();
    }
    
}