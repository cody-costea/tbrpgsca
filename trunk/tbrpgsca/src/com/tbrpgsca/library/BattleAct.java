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
    int pMb[] = {0,0,0, 0,0,0,0};
    int target,difference,current,result=0;
    
    Spinner targetBox, skillBox,itemBox;
    ArrayAdapter<String> targetList,skillList,itemList;
    TextView skillCost, updText;
    Button Act,Use,Auto,Escape;
    ImageView imgActor[] = new ImageView[7];

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_battle);
		imgActor[0]=(ImageView) findViewById(R.id.ImgPlayer);
		imgActor[1]=(ImageView) findViewById(R.id.ImgPlayer2);
		imgActor[2]=(ImageView) findViewById(R.id.ImgPlayer1);
		imgActor[3]=(ImageView) findViewById(R.id.ImgEnemy1);
		imgActor[4]=(ImageView) findViewById(R.id.ImgEnemy2);
		imgActor[5]=(ImageView) findViewById(R.id.ImgEnemy3);
		imgActor[6]=(ImageView) findViewById(R.id.ImgEnemy4);
		targetBox=(Spinner) findViewById(R.id.TargetBox);
		skillBox=(Spinner) findViewById(R.id.SkillBox);
		itemBox=(Spinner) findViewById(R.id.ItemBox);
		skillCost=(TextView) findViewById(R.id.SkillCost);
		updText=(TextView) findViewById(R.id.ItemCost);
		Act=(Button) findViewById(R.id.ActBt);
		Use=(Button) findViewById(R.id.UseBt);
		Escape=(Button) findViewById(R.id.RunBt);
		Auto=(Button) findViewById(R.id.AutoBt);
		skillList=new ArrayAdapter<String>(this,R.layout.simple_spinner_item);
		itemList=new ArrayAdapter<String>(this,R.layout.simple_spinner_item);
		targetList=new ArrayAdapter<String>(this,R.layout.simple_spinner_item);
		targetBox.setAdapter(targetList);
		skillBox.setAdapter(skillList);
		itemBox.setAdapter(itemList);
		imgActor[0].setOnClickListener(cAction);
		imgActor[1].setOnClickListener(cAction);
		imgActor[2].setOnClickListener(cAction);
		imgActor[3].setOnClickListener(cAction);
		imgActor[4].setOnClickListener(cAction);
		imgActor[5].setOnClickListener(cAction);
		imgActor[6].setOnClickListener(cAction);
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
        beginBattle(data.party,data.enemy[e],(extra!=null&&extra.containsKey("Surprise"))?extra.getBoolean("Surprise"):false);
	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		// Inflate the menu; this adds items to the action bar if it is present.
		getMenuInflater().inflate(R.menu.battle, menu);
		return true;
	}
	
    public void beginBattle(int party[],int enemy[], boolean surprise) {
    	difference=0;
    	int j=0;
    	for (int i=0;i<pMb.length;i++) pMb[i]=0;
    	for (int i=0;i<party.length;i++)
    		if (Player[party[i]].maxhp>0) {pMb[j]=party[i];j++;}
    		else difference++;
    	j=party.length;
    	for (int i=0;i<enemy.length;i++)
    		if (Player[enemy[i]].maxhp>0) {pMb[j]=enemy[i];j++;}
        for (int i=0;i<imgActor.length;i++) {
        	playSpr(i,0);
        }
        if (surprise) {current=2-difference;endTurn();} else {current=6;endTurn();}
        refreshTargetBox();
        targetBox.setSelection(3-difference);
        refreshSkillBox();
        refreshItemBox();
    }
    
    private void refreshTargetBox() {
    	targetList.clear();
    	String s;
        for (int i=0;i<pMb.length;i++) {
            if (Player[pMb[i]].maxhp>0){
            	s=Player[pMb[i]].name+" (HP: "+Player[pMb[i]].hp;
            	if (i<3-difference) s+=", MP: "+Player[pMb[i]].mp+", SP: "+Player[pMb[i]].sp;
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
            if (ability.contains(Skill[i].name))
            	{skill=i; break;}
    	return skill;
    }
    
    private int getItem() {
    	int skill=0;
    	String ability = (String)itemBox.getSelectedItem();
    	for (int i=0;i<Item.length;i++)
    		if (ability.contains(Item[i].name))
            	{skill=i;break;}
    	return skill;
    }
    
    private void executeSkill() {    	
    	executeSkill(Skill[getSkill()]);
    }
    
    private void executeSkill(Ability ability) {
    	if (target==2&&Player[pMb[1]].hp>0&&Player[pMb[0]].hp>0&&current>2&&difference==0&&!ability.range) target=(Player[pMb[0]].hp<Player[pMb[1]].hp)?0:1;
    	if (target==4-difference&&Player[pMb[6]].hp>0&&(Player[pMb[5]].hp>0||Player[pMb[3]].hp>0)&&!ability.range) target=6;
        if (target==5-difference&&(Player[pMb[4]].hp>0||Player[pMb[6]].hp>0)&&Player[pMb[3]].hp>0&&!ability.range) target=3;
        while ((Player[pMb[target]].hp<1&&ability.hpdmg>0)||(Player[pMb[target]].hp<1&&!ability.state[0]&&!Player[pMb[target]].active)) {
        	if (ability.hpdmg<0) target--; else target++;
        	if (target<0) target=2-difference;
            if (target>6-difference) target=3-difference;
        }
        int a=target,b=target;
        if (ability.trg>1) {a=0;b=6;}
        if (current>2-difference&&ability.trg<-1) {a=3;b=6;}
        if (current<3-difference&&ability.trg<-1) {a=0;b=2;}
        if (target>2-difference&&ability.trg==1) {a=3;b=6;}
        if (target<3-difference&&ability.trg==1) {a=0;b=2;}        
        if (ability.trg==-1) {a=current;b=current;}
        updText.append("\n"+Player[pMb[current]].name + " performs " + ability.name);
        playSpr(current,2);
        Player[pMb[current]].hp -= ability.hpc;
        Player[pMb[current]].mp -= ability.mpc;
        Player[pMb[current]].sp -= ability.spc;
        if (ability.qty>0) ability.qty--;
        for (int i=a;i<=b;i++) if (Player[pMb[i]].hp>0||ability.state[0]){
        	if (i!=current) playSpr(i,3);
        	updText.append(ability.execute(Player[pMb[current]], Player[pMb[i]]));
        	if (Player[pMb[i]].hp<1)
            	playSpr(i,1);
        }        
        if (Player[pMb[current]].hp<1)
        	playSpr(current,1);
    	if (current<3-difference) {Player[pMb[current]].exp++; Player[pMb[current]].levelUp();}
    	//updText.append(Player[pMb[current]].applyState(true));
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
    	int a = (current>2-difference) ? 3-difference : 0;
    	int b = (current<3-difference) ? 2-difference : 6-difference;
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
    	if (((current>2-difference)&&(ability.hpdmg>0))||((current<3-difference)&&(ability.hpdmg<1))) {a=0;b=2;}
    		else {a=3;b=6;}
    	if (Player[pMb[current]].auto<0) if (a==3) {a=0;b=2;} else {a=3;b=6;}
    	target=a;
    	while ((Player[pMb[target]].hp<1)&&(ability.hpdmg>1)) target++;
    	
    	for (int i=target;i<=b;++i)
    		if ((Player[pMb[i]].hp<Player[pMb[target]].hp)&&(Player[pMb[i]].hp>0))
    			target = i;
    	executeSkill(ability);
    }
    
    @SuppressWarnings("deprecation")
    private void endTurn() {
    		imgActor[current].setBackgroundDrawable(null);
    		current++;
    	if (current>6) {
        	current=0;
        	for (int i=0;i<pMb.length;i++)
            	if (Player[pMb[i]].hp>0){
            		Player[pMb[i]].active=true;
            		updText.append(Player[pMb[i]].applyState(true));
            		if (Player[pMb[i]].hp<1) playSpr(i,1);
            	}
        }
    	if (!checkEnd()){
    		if (Player[pMb[current]].active){	
    			if (current<3){
    				skillCost.setText(Player[pMb[current]].name+": "+Player[pMb[current]].hp+"/"+Player[pMb[current]].maxhp+" HP, "
    					+Player[pMb[current]].mp+"/"+Player[pMb[current]].maxmp+" MP, "
    					+Player[pMb[current]].sp+"/"+Player[pMb[current]].maxsp+" SP");
    			imgActor[current].setBackgroundResource(R.drawable.current);
    		}
    		Player[pMb[current]].applyState(false);
    		if (current<3) {refreshSkillBox();refreshItemBox();}
    		if ((current>2||Player[pMb[current]].auto!=0)&&Player[pMb[current]].hp>0) {
    			executeAI();endTurn();}
    		refreshTargetBox();
        	} else endTurn();
    	}
    }
    
    private boolean checkEnd(){
    	boolean b=false;
    	if ((Player[pMb[0]].hp<1)&&(Player[pMb[1]].hp<1)&&(Player[pMb[2]].hp<1))
    		{endBt(-1);b=true;} else
        if ((Player[pMb[3]].hp<1)&&(Player[pMb[4]].hp<1)&&(Player[pMb[5]].hp<1)&&(Player[pMb[6]].hp<1))
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
    	if (!Player[pMb[c]].bSprite[0].startsWith("b__")){
    	AnimationDrawable sprAnim=new AnimationDrawable();
    	int a,b;    	
    	switch (s){
    	  	case 2:
    	   		a=0;
    	   		b=7;
    	   	break;
    	   	case 3:
    	   		a=8;
    	   		b=13;
    	   	break;
    	   	case 1:
    	   		a=14;
    	   		b=21;
    	   	break;
    	   	default:
    	   		a=8;
    	   		b=8;
    	   	}
    	   	if (c>2) {a+=22;b+=22;}
    		for (int i=a;i<=b;i++){sprAnim.addFrame(Player[pMb[c]].getBtSprite(i,this), 87);    		
    	}
    	if (s>1) {a=(c>2)?30:8;sprAnim.addFrame(Player[pMb[c]].getBtSprite(a,this), 0);}
		imgActor[c].setImageDrawable(sprAnim);
		sprAnim.start();
		sprAnim.setOneShot(true);}
    }
    
    private boolean imgClick(int i){
    	boolean a=false;
    	int s=(i>2)?i-difference:i;
    	if (target==s){
			executeSkill(Skill[getAIskill((i>2-difference)?false:true)]);
			a=true;
    	}
		else			
			if (Player[pMb[i]].maxhp>0) targetBox.setSelection(s);
    	return a;
    }

    private OnClickListener cAction = new OnClickListener() {
    	@Override
    	public void onClick(final View v){
    		boolean act=true;
    		target = targetBox.getSelectedItemPosition();
    		if ((target>1&&difference>0)||(target>0&&difference>1)) target+=difference;
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
    			case R.id.ImgPlayer:             
    				act=imgClick(0);
    			break;
    			case R.id.ImgPlayer2:             
    				act=imgClick(1);
    			break;
    			case R.id.ImgPlayer1:             
    				act=imgClick(2);
    			break;
    			case R.id.ImgEnemy1:             
    				act=imgClick(3);
    			break;
    			case R.id.ImgEnemy2:             
    				act=imgClick(4);
    			break;
    			case R.id.ImgEnemy3:
    				act=imgClick(5);
    			break;
    			case R.id.ImgEnemy4:
    				act=imgClick(6);
    			break;
    		}
    		if (act) endTurn();
    	}
    };        
}