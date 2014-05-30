/*This file has been written by Claudiu-Stefan Costea,
 *and is released under the terms of the LGPL-2.1
 */
package com.tbrpgsca.library;

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

	Actor[] Player;
    Ability[] Skill;
    Ability[] Item;
    int pMb[] = {0,0,0,0, 0,0,0,0},waitTime[]={0,0};
    int target,difference,current=pMb.length-1,result=0;
    
    boolean surprised=false,waiting=false;
    
    Spinner targetBox, skillBox,itemBox;
    ArrayAdapter<String> targetList,skillList,itemList;
    TextView skillCost, updText;
    Button Act,Use,Auto,Escape;
    ImageView imgActor[] = new ImageView[8];
    
    AnimationDrawable sprAnim;
    
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
		DataApp data=(DataApp)this.getApplication();
		Player = data.Player;
	    Skill = data.Skill;
	    Item = data.Item;
	    Bundle extra = getIntent().getExtras();	    
	    Escape.setEnabled((extra!=null&&extra.containsKey("Escape"))?extra.getBoolean("Escape"):true);
	    int e=(extra!=null&&extra.containsKey("Enemy"))?extra.getInt("Enemy"):0;
        beginBattle(data.party,data.enemy[e],(extra!=null&&extra.containsKey("Surprise"))?extra.getInt("Surprise"):0);
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
    	for (int i=0;i<pMb.length;i++) pMb[i]=0;
    	for (int i=0;i<party.length;i++)
    		if (Player[party[i]].maxhp>0) {pMb[j]=party[i];j++;}
    		else difference++;
    	j=party.length;
    	int k=0;
    	for (int i=Player.length-4;i<Player.length;i++)
    		{Player[i].copy(Player[enemy[k]]);Player[i].auto=2;k++;pMb[j]=i;j++;}    	
        for (int i=0;i<imgActor.length;i++) {imgActor[i].setOnClickListener(cAction);playSpr(i,0,false);}
        if (surprise<0) {for(int i=0;i<(pMb.length/2)-difference;i++)Player[pMb[i]].active=false;endTurn();}
        	else if (surprise>0) {for(int i=(pMb.length/2);i<pMb.length;i++)Player[pMb[i]].active=false;endTurn();}
        		else endTurn();
        refreshTargetBox();
        targetBox.setSelection((pMb.length/2)-difference);
        refreshSkillBox();
        refreshItemBox();
    }
    
    private void refreshTargetBox() {
    	targetList.clear();
    	String s;
        for (int i=0;i<pMb.length;i++) {
            if (Player[pMb[i]].maxhp>0){
            	s=Player[pMb[i]].name+" (HP: "+Player[pMb[i]].hp;
            	if (i<(pMb.length/2)-difference) s+=", MP: "+Player[pMb[i]].mp+", SP: "+Player[pMb[i]].sp;
            	s+=")";
                targetList.add(s);
            }
        }
        targetList.notifyDataSetChanged();
    }
    private void refreshSkillBox() {
        skillList.clear();
        String trg;
        for (int i=0;i<Player[pMb[current]].skill.size();i++)
            if (!(Skill[Player[pMb[current]].skill.get(i)].lvrq>Player[pMb[current]].level)&&!(Skill[Player[pMb[current]].skill.get(i)].mpc>Player[pMb[current]].mp)&&
                    !(Skill[Player[pMb[current]].skill.get(i)].spc>Player[pMb[current]].sp)&&!(Skill[Player[pMb[current]].skill.get(i)].hpc>=Player[pMb[current]].hp)){
            	trg=(Skill[Player[pMb[current]].skill.get(i)].trg>0)?"All":"Self"; if (Skill[Player[pMb[current]].skill.get(i)].trg==0) trg="One";
                        skillList.add(Skill[Player[pMb[current]].skill.get(i)].name+" (Rq:" + Skill[Player[pMb[current]].skill.get(i)].hpc + "HP," + Skill[Player[pMb[current]].skill.get(i)].mpc +
                        		"MP," + Skill[Player[pMb[current]].skill.get(i)].spc + "SP;" + "Trg:"+trg+")");}
        skillList.notifyDataSetChanged();
        if (skillBox.getSelectedItemPosition()>1) skillBox.setSelection(0);
    }
    
    private void refreshItemBox() {
        itemList.clear();
        String trg;
        for (int i=0;i<Item.length;i++)
            if (Item[i].qty>0){trg=(Item[i].trg>0)?"All":"Self"; if (Item[i].trg==0) trg="One";
                itemList.add(Item[i].name+" (Qty:"+Item[i].qty+";" + "Trg:"+trg+")");
        }
        if (itemList.getCount()==0) { Use.setEnabled(false); itemBox.setEnabled(false);}
        	else { Use.setEnabled(true); itemBox.setEnabled(true); }
        itemList.notifyDataSetChanged();
    }    
    
    private int getSkill() {
    	int skill=0;
    	String ability = (String)skillBox.getSelectedItem();
    	for (int i=0;i<Skill.length;i++)
            if (ability.startsWith(Skill[i].name))
            	{skill=i; break;}
    	return skill;
    }
    
    private int getItem() {
    	int skill=0;
    	String ability = (String)itemBox.getSelectedItem();
    	for (int i=0;i<Item.length;i++)
    		if (ability.startsWith(Item[i].name))
            	{skill=i;break;}
    	return skill;
    }
    
    private void executeSkill() {    	
    	executeSkill(Skill[getSkill()]);
    }
    
    private void executeSkill(Ability ability) {
    	if (target==1&&Player[pMb[3]].hp>0&&(Player[pMb[0]].hp>0||Player[pMb[2]].hp>0)&&current>(pMb.length/2)-1&&difference==0&&!ability.range) target=3;
    	if (target==2&&Player[pMb[0]].hp>0&&(Player[pMb[1]].hp>0||Player[pMb[3]].hp>0)&&current>(pMb.length/2)-1&&difference<2&&!ability.range) target=0;
    	if (target==4&&Player[pMb[6]].hp>0&&(Player[pMb[5]].hp>0||Player[pMb[7]].hp>0)&&!ability.range) target=6;
        if (target==7&&(Player[pMb[6]].hp>0||Player[pMb[4]].hp>0)&&Player[pMb[5]].hp>0&&!ability.range) target=5;
        while ((Player[pMb[target]].hp<1&&ability.hpdmg>0)||(Player[pMb[target]].hp<1&&!ability.state[0]&&!Player[pMb[target]].active)) {
        	if (ability.hpdmg<0) target--; else target++;
        	if (target<0) target=(pMb.length/2)-1-difference;
            if (target>pMb.length-difference) target=(pMb.length/2)-difference;
        }
        int a=target,b=target;
        if (ability.trg>1) {a=0;b=6;}
        if (current>(pMb.length/2)-1-difference&&ability.trg<-1) {a=(pMb.length/2);b=pMb.length-1;}
        if (current<(pMb.length/2)-difference&&ability.trg<-1) {a=0;b=(pMb.length/2)-1;}
        if (target>(pMb.length/2)-1-difference&&ability.trg==1) {a=(pMb.length/2);b=pMb.length-1;}
        if (target<(pMb.length/2)-difference&&ability.trg==1) {a=0;b=(pMb.length/2)-1;}        
        if (ability.trg==-1) {a=current;b=current;}
        updText.append("\n"+Player[pMb[current]].name + " performs " + ability.name);
        Player[pMb[current]].hp -= ability.hpc;
        Player[pMb[current]].mp -= ability.mpc;
        Player[pMb[current]].sp -= ability.spc;
        if (ability.qty>0) ability.qty--;
        int trg;
    	for (int i=a;i<=b;i++) if (Player[pMb[i]].hp>0||ability.state[0]){        	
        	if (Player[pMb[i]].reflect&&ability.dmgtype==2) {
        		updText.append(", which is reflected");
        		trg=current;
        	} else trg=i;
        	if (i==a) playSpr(current,3,false);
        	if (trg!=current) playSpr(trg,1,true);
        	waitTime[0]=waitTime[1];
        	updText.append(ability.execute(Player[pMb[current]], Player[pMb[trg]]));
        	if (current!=trg&&Player[pMb[trg]].hp<1)
            	playSpr(trg,2,false);
        }        
        if (Player[pMb[current]].hp<1)
        	playSpr(current,2,false);
    	if (current<(pMb.length/2)-difference) {Player[pMb[current]].exp++; Player[pMb[current]].levelUp();}
    	updText.append(".");
    }
    
    private void useItem() {
        executeSkill(Item[getItem()]);
    }
    
    private void executeAI() {
    	setAItarget(Skill[getAIskill(checkAIheal())]);
    }
    
    private boolean checkAIheal() {
    	boolean nHeal=false;
    	int a = (current>(pMb.length/2)-1-difference) ? (pMb.length/2)-difference : 0;
    	int b = (current<(pMb.length/2)-difference) ? (pMb.length/2)-1-difference : (pMb.length/2)-1-difference;
    	for (int i=a;i<=b;i++)
    		if (Player[pMb[i]].hp<(Player[pMb[i]].maxhp/3)) {
    			nHeal=true;
    			break;
    		}
    	if (nHeal) {
    		nHeal=false;
    		for (int i=0;i<Player[pMb[current]].skill.size();i++) {
    			a=Player[pMb[current]].skill.get(i);
    			if (Skill[a].hpdmg<0&&Skill[a].mpc<=Player[pMb[current]].mp&&Skill[a].hpc<=Player[pMb[current]].hp&&
        				Skill[a].spc<=Player[pMb[current]].sp&&Player[pMb[current]].level>=Skill[a].lvrq) {
    				nHeal=true;break;
    			}
    		}
    	}
    	return nHeal;
    }
    
    private int getAIskill(boolean heal) {
    	int s=(heal) ? 1:0;
    	int a;
    	for (int i=0;i<Player[pMb[current]].skill.size();i++) {
    		a=Player[pMb[current]].skill.get(i);
    		if (Skill[a].mpc<=Player[pMb[current]].mp&&Skill[a].hpc<Player[pMb[current]].hp&&
    				Skill[a].spc<=Player[pMb[current]].sp&&Player[pMb[current]].level>=Skill[a].lvrq)
    			if (heal) {if (Skill[a].hpdmg<Skill[s].hpdmg&&Skill[a].hpdmg<0) s=a;}
    			else if (Skill[a].hpdmg>Skill[s].hpdmg) s=a;
    	}
    	return s;
    }
    
    private void setAItarget(Ability ability) {
    	int a;
    	int b;
    	if (((current>(pMb.length/2)-1-difference)&&(ability.hpdmg>0))||((current<(pMb.length/2)-difference)&&(ability.hpdmg<1))) {a=0;b=(pMb.length/2)-1;}
    		else {a=(pMb.length/2);b=pMb.length-1;}
    	if (Player[pMb[current]].auto<0) if (a==(pMb.length/2)) {a=0;b=(pMb.length/2)-1;} else {a=(pMb.length/2);b=pMb.length-1;}
    	target=a;
    	while ((Player[pMb[target]].hp<1)&&(ability.hpdmg>1)&&target<b) target++;
    	
    	for (int i=target;i<=b;++i)
    		if ((Player[pMb[i]].hp<Player[pMb[target]].hp)&&(Player[pMb[i]].hp>0))
    			target = i;
    	executeSkill(ability);
    }
    
    @SuppressWarnings("deprecation")
    private void endTurn() {
    	imgActor[current].setBackgroundDrawable(null);
    	Player[pMb[current]].active=false;

    	boolean reset=true;
    	for (int i=0;i<pMb.length;i++){
    		if (Player[pMb[i]].active) {reset=false;break;}
    	}
    	if (reset) {
        	for (int i=0;i<pMb.length;i++)
            	if (Player[pMb[i]].hp>0){
            		updText.append(Player[pMb[i]].applyState(true));
            		if (Player[pMb[i]].hp<1) playSpr(i,2,false);
            	}
        }
    	for (current=0;current<pMb.length;current++){
    		if (Player[pMb[current]].active) break;
    	}
    	if (current<6)
    		for (int i=current+1;i<pMb.length;i++)
    			if ((Player[pMb[current]].agi<Player[pMb[i]].agi)&&Player[pMb[i]].active)
    				current=i;
    		
    	if (!checkEnd()){
    		if (Player[pMb[current]].active){	
    			if (Player[pMb[current]].auto==0){
    				setCurrent();
    				refreshItemBox();
    		}
    		Player[pMb[current]].applyState(false);
    		if (Player[pMb[current]].auto!=0&&Player[pMb[current]].hp>0) {
    			executeAI();endTurn();}
    		refreshTargetBox();
        	} else endTurn();
    	}
    }
    
    private void setCurrent(){
    	skillCost.setText(Player[pMb[current]].name+": "+Player[pMb[current]].hp+"/"+Player[pMb[current]].maxhp+" HP, "
				+Player[pMb[current]].mp+"/"+Player[pMb[current]].maxmp+" MP, "
				+Player[pMb[current]].sp+"/"+Player[pMb[current]].maxsp+" SP");
		imgActor[current].setBackgroundResource(R.drawable.current);
		refreshSkillBox();
    }
    
    private boolean checkEnd(){
    	boolean b=false;
    	if ((Player[pMb[0]].hp<1)&&(Player[pMb[1]].hp<1)&&(Player[pMb[2]].hp<1)&&(Player[pMb[3]].hp<1))
    		{endBt(-1);b=true;} else
        if ((Player[pMb[4]].hp<1)&&(Player[pMb[5]].hp<1)&&(Player[pMb[6]].hp<1)&&(Player[pMb[7]].hp<1))
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
    
    private void playSpr(final int c, final int s, final boolean bkw){
    	if (Player[pMb[c]].bsprite!=""){
    		final String pos=((c>(pMb.length/2)-1&&!surprised)||(c<(pMb.length/2)&&surprised))?"r":"l";    		
    		final AnimationDrawable sprAnim=Player[pMb[c]].getBtSprite(s, pos, bkw, waitTime, this);    		
    		imgActor[c].postDelayed(new Runnable(){
						@Override
						public void run() {							
				    		sprAnim.setOneShot(true);				    		
		    				imgActor[c].setImageDrawable(sprAnim);
							sprAnim.start();
						}					
    		}, waitTime[0]);
    		//imgActor[c].setTag(s);
    	} 	 
    }
    
    private boolean imgClick(int i){
    	boolean a=false;    	
    	int s=(i>(pMb.length/2)-1)?i-difference:i;
    	if (target==i){
    		executeSkill(Skill[getAIskill((i>pMb.length/2-1&&current<pMb.length/2)||(i<pMb.length/2&&current>pMb.length/2-1)?false:true)]);
			a=true;}
		else			
			if (Player[pMb[i]].maxhp>0) targetBox.setSelection(s);
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