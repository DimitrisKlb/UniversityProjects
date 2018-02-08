import javax.swing.*;
import java.awt.*;
import java.awt.event.*;

public class SnakePanel extends JPanel implements Runnable{
    private Snake s;  
    private Thread runner;
    private Map m;      
    private boolean begin = true;
    private boolean lost = true;
    boolean readyToChangeDir = false;
    private long speed = 80;
    private long score = 0;
    private int size = 3;
    private int size_dif = (size-1)/2;
    
    public SnakePanel(){
        super();
        s = new Snake(181,100,181,52,Snake.RIGHT);
        m = new Map(288,264,3); 
    }     
    
    public void startMoving(){
        if (runner == null); {
            runner = new Thread(this);
            runner.start();
        }
    }
    public void stopMoving(){
        if (runner != null) {
            runner.stop();
            runner = null;
        }
    }
    public void run(){        
        while (true){             
            changeHead();
            m.setCell(s.getHeadH(),s.getHeadW(),Map.SNAKE);
            if((m.cell(s.getTailH(),s.getTailW())>10)&&(m.cell(s.getTailH(),s.getTailW())<20))
                m.setCell(s.getTailH(),s.getTailW(),Map.EMPTY);
            changeTail();
            readyToChangeDir = true;
            
            repaint();
            try { Thread.sleep(speed); }
            catch (InterruptedException e) { }            
        }    
    } 
    public void update(Graphics g){
        paint(g);    
    }
    public void paint(Graphics g){
        if(begin){
            g.setColor(Color.BLACK);
            g.fillRect(2, 2, this.getWidth()-4, this.getHeight()-4);
            g.setColor(Color.YELLOW);
            g.setFont(new Font("SansSerif",Font.BOLD,27));
            g.drawString("PC    NAKE",60,80);            
            g.setFont(new Font("SansSerif",Font.BOLD,20));
            g.drawString("Version Viper",70,120);   
            g.setFont(new Font("SansSerif",Font.PLAIN,15));
            g.drawString("(Press 'New Game' to Start)",40,270);
            g.setColor(Color.GREEN);
            g.setFont(new Font("Imprint MT Shadow",Font.BOLD,40));
            g.drawString("S",105,80);             
        }else{
            for(int i=0;i<m.mHeight();i++){
                for(int k=0;k<m.mWidth() ;k++){
                    switch(m.cell(i,k)){
                        case Map.EMPTY: 
                            g.setColor(Color.WHITE);
                            break;
                        case Map.SNAKE:
                            g.setColor(Color.BLACK);
                            break;
                        case Map.FOOD:
                            g.setColor(Color.RED);
                            break;    
                        case Map.WALL:
                            g.setColor(Color.BLUE);
                            break;
                        case Map.WARP:
                            g.setColor(Color.RED);
                            break; 
                        case Map.SPECIAL:
                            g.setColor(Color.PINK);
                            break;
                        }        
                    if( (m.cell(i,k)>11)&&(m.cell(i,k)<20) ){g.setColor(Color.BLACK);}
                    g.drawLine(k,i,k,i);
                }
            }
            g.setFont(new Font("SansSerif",Font.PLAIN,15));
            g.setColor(Color.WHITE);            
            g.fillRect(0,288,264,21);
            g.setColor(Color.BLACK);            
            g.drawString("Score: "+score,104,303);
        }           
    }
    public Snake getSnake(){return this.s;}
    public Thread getThread(){return this.runner;}
    public Map getMap(){return this.m;}
    public boolean hasLost(){return this.lost;}
    public long getScore(){return this.score;}
    public void setScore(long score){this.score = score;}
    public void setLost(boolean lost){this.lost = lost;}
    
    public void setMap(){        
        for(int i=1;i<m.mWidth()-1;i++){
            m.setCell(1,i,Map.WALL); 
            m.setCell(m.mHeight()-2,i,Map.WALL); 
            m.setCell(4,i,Map.WALL); 
            m.setCell(m.mHeight()-5,i,Map.WALL);
        }
        for(int i=1;i<m.mHeight()-1;i++){
            m.setCell(i,1,Map.WALL);
            m.setCell(i,m.mWidth()-2,Map.WALL);
            m.setCell(i,4,Map.WALL);
            m.setCell(i,m.mWidth()-5,Map.WALL);
        }
        for(int i = s.getHeadW();i>s.getTailW();i=i-3){            
            m.setCell(s.getHeadH(),i,Map.SNAKE);
        }  
        setNewFood();
        begin = false;
        repaint();
    }
    public void clearMap(){
        for(int i=0;i<m.mHeight();i++){
            for(int k=0;k<m.mWidth();k++){
                m.setSquare(i,k,Map.EMPTY);
            }    
        } 
        begin = false;
        repaint();
    }
    public void changeHead(){
        int dh=0,dw=0;
        switch(s.getDirectionH()){
            case Snake.UP:    
                dh =  -size;
                dw =  0;
                break;
            case Snake.RIGHT:
                dh =  0;
                dw =  size;            
                break;
            case Snake.DOWN:
                dh =  size;
                dw =  0;            
                break;
            case Snake.LEFT:
                dh =  0;
                dw = -size;            
                break;
        }
        check4Collision(dh,dw);
        s.setHeadH(s.getHeadH() + dh);
        s.setHeadW(s.getHeadW() + dw);
    }    
        
    public void changeTail(){
        int ch=0,cw=0;
        switch(s.getDirectionT()){
            case Snake.UP:    
                ch =  -size;
                cw =  0;
                break;
            case Snake.RIGHT:
                ch =  0;
                cw =  size;            
                break;
            case Snake.DOWN:
                ch =  size;
                cw =  0;            
                break;
            case Snake.LEFT:
                ch =  0;
                cw = -size;            
                break;
        }    
        if( m.cell(s.getTailH()+ch,s.getTailW()+cw) > 10 ){
            switch(  m.cell(s.getTailH()+ch,s.getTailW()+cw)  ){
            case Map.BRANCH_UP:    
                s.setDirectionT(Snake.UP);                
                break;
            case Map.BRANCH_RIGHT:
                s.setDirectionT(Snake.RIGHT);                
                break;
            case Map.BRANCH_DOWN:
                s.setDirectionT(Snake.DOWN);
                break;
            case Map.BRANCH_LEFT:
                s.setDirectionT(Snake.LEFT);                
                break;                
            } 
        }   
        s.setTailH(s.getTailH() + ch);
        s.setTailW(s.getTailW() + cw);    
    } 
    
    public void check4Collision(int dh,int dw){
        switch(m.cell(s.getHeadH()+dh,s.getHeadW()+dw) ){
            case Map.FOOD:
                int ch=0,cw=0;
                switch(s.getDirectionT()){
                    case Snake.UP:    
                        ch =  -size;
                        cw =  0;
                        break;
                    case Snake.RIGHT:
                        ch =  0;
                        cw =  size;            
                        break;
                    case Snake.DOWN:
                        ch =  size;
                        cw =  0;            
                        break;
                    case Snake.LEFT:
                        ch =  0;
                        cw = -size;            
                        break;
                }
                s.setTailH(s.getTailH()-ch);
                s.setTailW(s.getTailW()-cw);
                s.setLength(s.getLength() + 1);
                this.score++;                 
                setNewFood();
                break;
            case Map.WALL:
            case Map.SNAKE:
                lose();                               
                break;
            case Map.WARP:
                switch(s.getDirectionH()){
                    case Snake.UP:
                    
                        break;
                    case Snake.RIGHT:
                    
                        break;
                    case Snake.DOWN:
                    
                        break;
                    case Snake.LEFT:
                    
                        break;
                }                             
        }    
    }
    public void lose(){
        String sPoints = " point";
        if(score!=1) sPoints += "s";        
        JOptionPane.showMessageDialog(this,"You have scored "+score+ sPoints,"Game Over!",JOptionPane.INFORMATION_MESSAGE);
        lost = true;
        stopMoving();         
    }
    public void setNewFood(){
        java.util.Random rand = new java.util.Random(); 
        int randH=0,randW=0;
        boolean foodOk = false;
        
        while(!foodOk){
            randH = rand.nextInt(500)%(m.mHeight()-15)+6;    
            randW = rand.nextInt(500)%(m.mWidth()-15)+6; 
            if( (m.cell(randH,randW) == Map.EMPTY)&&(randH%3==1)&&(randW%3==1)){
                m.setCell(randH,randW,Map.FOOD);
                foodOk = true;    
            }
        }        
    } 
    public void resetGame(){ 
        setLost(false);
        s.resetPosition();
        this.score = 0;
        clearMap();
        setMap();        
    }
    public void updateScoreboard(){}
        
}
