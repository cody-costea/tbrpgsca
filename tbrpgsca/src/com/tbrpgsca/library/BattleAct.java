/*This file has been written by Claudiu-Stefan Costea,
 *and is released under the terms of the LGPL-2.1
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
import android.view.Menu;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.Spinner;
import android.widget.TextView;

public class BattleAct extends Activity {

	Ability[] Skill = DataApp.Skill;
    Ability[] Item = DataApp.Item;
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
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_battle);
		targetBox=(Spinner) findViewById(R.id.TargetBox);
		skillBox=(Spinner) findViewById(R.id.SkillBox);
		itemBox=(Spinner) findViewById(R.id.ItemBox);
		skillCost=(TextView) findViewById(R.id.SkillCost);
		updText=(TextView) findViewById(R.id.ItemCost);
		Act=(Button) findViewById(R.id.ActBt);
		Use=(Button) findViewById(R.id.UseBt);
		Escape=(Button) findViewById(R.id.RunBt);
		Auto=(Button) findViewById(R.id.AutoBt);
		skillList=new ArrayAdapter<String>(this,android.R.layout.simple_spinner_item);
		itemList=new ArrayAdapter<String>(this,android.R.layout.simple_spinner_item);
		targetList=new ArrayAdapter<String>(this,android.R.layout.simple_spinner_item);
		targetBox.setAdapter(targetList);
		skillBox.setAdapter(skillList);
		itemBox.setAdapter(itemList);
		Act.setOnClickListener(cAction);
		Use.setOnClickListener(cAction);
		Escape.setOnClickListener(cAction);
		Auto.setOnClickListener(cAction);
		updText.setMovementMethod(new ScrollingMovementMethod());
		Bundle extra = getIntent().getExtras();	    
	    Escape.setEnabled((extra!=null&&extra.containsKey("Escape"))?extra.getBoolean("Escape"):true);
	    int e=(extra!=null&&extra.containsKey("Enemy"))?extra.getInt("Enemy"):0;
        beginBattle(DataApp.party,DataApp.enemy[e],(extra!=null&&extra.containsKey("Surprise"))?extra.getInt("Surprise"):0);
	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		// Inflate the menu; this adds items to the action bar if it is present.
		getMenuInflater().inflate(R.menu.battle, menu);
		return true;
	}
	
    public void beginBattle(int party[],int enemy[], int surprise) {
    	this.surprised=(surprise<0);
    	if (surprised){
    		imgActor[0]=(ImageView) findViewById(R.id.ImgEnemy3);
    		imgActor[1]=(ImageView) findViewById(R.id.ImgEnemy4);
    		imgActor[2]=(ImageView) findViewById(R.id.ImgEnemy1);
    		imgActor[3]=(ImageView) findViewById(R.id.ImgEnemy2);
    		imgActor[4]=(ImageView) findViewById(R.id.ImgPlayer3);
    		imgActor[5]=(ImageView) findViewById(R.id.ImgPlayer4);
    		imgActor[6]=(ImageView) findViewById(R.id.ImgPlayer1);
    		imgActor[7]=(ImageView) findViewById(R.id.ImgPlayer2);
    	} else{
    		imgActor[0]=(ImageView) findViewById(R.id.ImgPlayer1);
    		imgActor[1]=(ImageView) findViewById(R.id.ImgPlayer2);
    		imgActor[2]=(ImageView) findViewById(R.id.ImgPlayer3);
    		imgActor[3]=(ImageView) findViewById(R.id.ImgPlayer4);
    		imgActor[4]=(ImageView) findViewById(R.id.ImgEnemy1);
    		imgActor[5]=(ImageView) findViewById(R.id.ImgEnemy2);
    		imgActor[6]=(ImageView) findViewById(R.id.ImgEnemy3);
    		imgActor[7]=(ImageView) findViewById(R.id.ImgEnemy4);
    	}
    	difference=0;
    	int j=0;
    	for (int i=0;i<party.length;i++){
    		Player[j]=DataApp.Player[party[i]];
    		if (Player[j].maxhp<1) difference++;
    		j++;
    	}
    	for (int k=0;k<enemy.length;k++)
    		{Player[j]=new Actor();Player[j].copy(DataApp.Player[enemy[k]]);Player[j].auto=2;j++;}    	
        for (int i=0;i<imgActor.length;i++) {imgActor[i].setOnClickListener(cAction);imgActor[i].setTag(0);playSpr(i,Player[i].hp>0?0:-1);}
        if (surprise<0) {for(int i=0;i<(Player.length/2)-difference;i++)Player[i].active=false;endTurn();}
        	else if (surprise>0) {for(int i=(Player.length/2);i<Player.length;i++)Player[i].active=false;endTurn();}
        		else endTurn();
        refreshTargetBox();
        targetBox.setSelection((Player.length/2)-difference);
        refreshSkillBox();
        refreshItemBox();
    }
    
    private void refreshTargetBox() {
    	targetList.clear();
    	String s;
        for (int i=0;i<Player.length;i++) {
            if (Player[i].maxhp>0){
            	s=Player[i].name+" (HP: "+Player[i].hp;
            	if (i<(Player.length/2)-difference) s+=", MP: "+Player[i].mp+", SP: "+Player[i].sp;
            	s+=")";
                targetList.add(s);
            }
        }
        targetList.notifyDataSetChanged();
    }
    private void refreshSkillBox() {
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
    
    private void refreshItemBox() {
        itemList.clear();itemId.clear();String trg;
        for (int i=0;i<Item.length;i++)
            if (Item[i].qty>0){
            	trg=(Item[i].trg>0)?"All":"Self"; if (Item[i].trg==0) trg="One"; itemId.add(i);
                itemList.add(Item[i].name+" (Qty:"+Item[i].qty+";" + "Trg:"+trg+")");
        }
        if (itemList.getCount()==0) { Use.setEnabled(false); itemBox.setEnabled(false);}
        	else { Use.setEnabled(true); itemBox.setEnabled(true); }
        itemList.notifyDataSetChanged();
    }    
    
    private void executeSkill() {    	
    	executeSkill(Player[current].skill.get(skillId.get(skillBox.getSelectedItemPosition())));
    }
    
    private void executeSkill(Ability ability) {
    	if (target==1&&Player[3].hp>0&&(Player[0].hp>0||Player[2].hp>0)&&current>(Player.length/2)-1&&difference==0&&!ability.range) target=3;
    	if (target==2&&Player[0].hp>0&&(Player[1].hp>0||Player[3].hp>0)&&current>(Player.length/2)-1&&difference<2&&!ability.range) target=0;
    	if (target==4&&Player[6].hp>0&&(Player[5].hp>0||Player[7].hp>0)&&!ability.range) target=6;
        if (target==7&&(Player[6].hp>0||Player[4].hp>0)&&Player[5].hp>0&&!ability.range) target=5;
        while (Player[target].hp<1&&!(ability.state[0]&&((ability.hpdmg<0&&Player[target].mres[ability.element]<7)||
        		(ability.hpdmg>0&&Player[target].mres[ability.element]==7)))) {
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
        updText.append("\n"+Player[current].name + " performs " + ability.name);
        Player[current].hp -= ability.hpc;
        Player[current].mp -= ability.mpc;
        Player[current].sp -= ability.spc;
        if (ability.qty>0) ability.qty--;
        int trg;boolean act=true;
    	for (int i=a;i<=b;i++) if (Player[i].hp>0||ability.state[0]){        	
        	if (Player[i].reflect&&ability.dmgtype==2&&i!=current) {
        		updText.append(", which is reflected");
        		trg=current;
        	} else trg=i;
        	if (act) {playSpr(current,3);act=false;}
        	updText.append(ability.execute(Player[current], Player[trg]));
        	boolean ko=Player[trg].hp<1;
        	if (trg!=current) playSpr(trg,ko?2:1);        	
        }
    	waitTime[0]=waitTime[1]>waitTime[2]?waitTime[1]:waitTime[2];
        if (Player[current].hp<1)
        	playSpr(current,2);
    	if (current<(Player.length/2)-difference) {Player[current].exp++; Player[current].levelUp();}
    	updText.append(".");
    }
    
    private void useItem() {
    	executeSkill(Item[itemId.get(itemBox.getSelectedItemPosition())]);
    }
    
    private void executeAI() {
    	setAItarget(getAIskill(checkAIheal()));
    }
    
    private boolean checkAIheal() {
    	boolean nHeal=false;
    	int a = (current>(Player.length/2)-1-difference) ? (Player.length/2)-difference : 0;
    	int b = (current<(Player.length/2)-difference) ? (Player.length/2)-1-difference : (Player.length/2)-1-difference;
    	for (int i=a;i<=b;i++)
    		if (Player[i].hp<(Player[i].maxhp/3)) {
    			nHeal=true;
    			break;
    		}
    	if (nHeal) {
    		nHeal=false;
    		for (int i=0;i<Player[current].skill.size();i++) {
    			Ability s=Player[current].skill.get(i);
    			if (s.hpdmg<0&&s.mpc<=Player[current].mp&&s.hpc<=Player[current].hp&&
        				s.spc<=Player[current].sp&&Player[current].level>=s.getLvRq()) {
    				nHeal=true;break;
    			}
    		}
    	}
    	return nHeal;
    }
    
    private Ability getAIskill(boolean heal) {
    	Ability s=(heal) ? Skill[1]:Skill[0];
    	Ability a;
    	for (int i=0;i<Player[current].skill.size();i++) {
    		a=Player[current].skill.get(i);
    		if (a.mpc<=Player[current].mp&&a.hpc<Player[current].hp&&
    				a.spc<=Player[current].sp&&Player[current].level>=a.getLvRq())
    			if (heal) {if (a.hpdmg<s.hpdmg&&a.hpdmg<0) s=a;}
    			else if (a.hpdmg>s.hpdmg) s=a;
    	}
    	return s;
    }
    
    private void setAItarget(Ability ability) {
    	int a;
    	int b;
    	if (((current>(Player.length/2)-1-difference)&&(ability.hpdmg>0))||((current<(Player.length/2)-difference)&&(ability.hpdmg<1))) {a=0;b=(Player.length/2)-1;}
    		else {a=(Player.length/2);b=Player.length-1;}
    	if (Player[current].auto<0) if (a==(Player.length/2)) {a=0;b=(Player.length/2)-1;} else {a=(Player.length/2);b=Player.length-1;}
    	target=a;
    	while ((Player[target].hp<1)&&(ability.hpdmg>1)&&target<b) target++;
    	
    	for (int i=target;i<=b;++i)
    		if ((Player[i].hp<Player[target].hp)&&(Player[i].hp>0))
    			target = i;
    	executeSkill(ability);
    }
    
    @SuppressWarnings("deprecation")
    private void endTurn() {
    	imgActor[current].setBackgroundDrawable(null);
    	Player[current].active=false;

    	boolean reset=true;
    	for (int i=0;i<Player.length;i++){
    		if (Player[i].active) {reset=false;break;}
    	}
    	if (reset) {
        	for (int i=0;i<Player.length;i++)
            	if (Player[i].hp>0){
            		updText.append(Player[i].applyState(true));
            		if (Player[i].hp<1) playSpr(i,2);
            	}
        }
    	for (current=0;current<Player.length;current++){
    		if (Player[current].active) break;
    	}
    	if (current<6)
    		for (int i=current+1;i<Player.length;i++)
    			if ((Player[current].agi<Player[i].agi)&&Player[i].active)
    				current=i;
    		
    	if (!checkEnd()){
    		if (Player[current].active){	
    			if (Player[current].auto==0){
    				setCurrent();
    				refreshItemBox();
    		}
    		Player[current].applyState(false);
    		if (Player[current].auto!=0&&Player[current].hp>0) {
    			executeAI();endTurn();}
    		refreshTargetBox();
        	} else endTurn();
    	}
    }
    
    private void setCurrent(){
    	skillCost.setText(Player[current].name+": "+Player[current].hp+"/"+Player[current].maxhp+" HP, "
				+Player[current].mp+"/"+Player[current].maxmp+" MP, "
				+Player[current].sp+"/"+Player[current].maxsp+" SP");
		imgActor[current].setBackgroundResource(R.drawable.current);
		refreshSkillBox();
    }
    
    private boolean checkEnd(){
    	boolean b=false;
    	if ((Player[0].hp<1)&&(Player[1].hp<1)&&(Player[2].hp<1)&&(Player[3].hp<1))
    		{endBt(-1);b=true;} else
        if ((Player[4].hp<1)&&(Player[5].hp<1)&&(Player[6].hp<1)&&(Player[7].hp<1))
        	{endBt(1);b=true;}
    	return b;
    }
    
    private void endBt(int r) {
    	String t="Defeat"; String s="The party has fled!";
    	if (r>0) {t="Victory"; s="The party has won the battle!";}
    	if (r<0) s="The party has been defeated!";
    	result=r;
    	endingMsg(t,s);
    }
    
    private void endingMsg(String t,String s){
    	AlertDialog.Builder msg = new AlertDialog.Builder(this);
    	msg.setCancelable(false);
    	msg.setMessage(s);
    	msg.setTitle(t);
    	msg.setPositiveButton("Exit", new DialogInterface.OnClickListener() {			
			@Override
			public void onClick(DialogInterface arg0, int arg1) {
				// TODO Auto-generated method stub
				Intent outcome = new Intent();
				outcome.putExtra("Outcome", result);
				setResult(RESULT_OK, outcome);
				finish();
			}
		});
    	AlertDialog dialog = msg.create();
    	dialog.show();
    }
    
    private void playSpr(final int c, final int s){
    	if (Player[c].getSprName()!=""){
    		final String pos=((c>(Player.length/2)-1&&!surprised)||(c<(Player.length/2)&&surprised))?"r":"l";    		
    		final AnimationDrawable sprAnim=Player[c].getBtSprite(s, pos, waitTime, this);    		
    		imgActor[c].postDelayed(new Runnable(){
				@Override
				public void run() {
					if (s==2||s<0||Player[c].hp>0||!imgActor[c].getTag().equals(2)){
						sprAnim.setOneShot(true);				    		
						imgActor[c].setImageDrawable(sprAnim);
						sprAnim.start();imgActor[c].setTag(s);				   		
					}
				}					
    		}, waitTime[0]);
    	} 	 
    }
    
    private boolean imgClick(int i){
    	boolean a=false;    	
    	int s=(i>(Player.length/2)-1)?i-difference:i;
    	if (target==i){
    		executeSkill(getAIskill((i>Player.length/2-1&&current<Player.length/2)||(i<Player.length/2&&current>Player.length/2-1)?false:true));
			a=true;}
		else			
			if (Player[i].maxhp>0) targetBox.setSelection(s);
    	return a;
    }

    private OnClickListener cAction = new OnClickListener() {
    	@Override
    	public void onClick(final View v){
    		waitTime[0]=0;
    		boolean act=true;
    		target = targetBox.getSelectedItemPosition();
    		if ((target>2&&difference>0)||(target>0&&difference>2)||(target>1&&difference>1)) target+=difference;
    		switch (v.getId()){
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
    public void onBackPressed(){
    	endBt(-1);
    }
}