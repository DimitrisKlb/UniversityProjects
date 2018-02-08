import javax.swing.*;
import java.awt.*;
import java.awt.event.*;
public class Interface extends JFrame implements ActionListener,KeyListener{
    private SnakePanel game;    
    private boolean paused = true;
    private boolean canPause = true;
    public Interface(String title){
        super(title);
        setDefaultCloseOperation(WindowConstants.EXIT_ON_CLOSE);
        game = new SnakePanel();          
        JPanel content = (JPanel)getContentPane();       
                  
        content.add(game);
        
        JMenuBar bar = new JMenuBar();
        JMenu opt = new JMenu("Options");
        JMenuItem newgame = new JMenuItem("New Game");
        JMenuItem pause = new JMenuItem("Pause");
        JMenuItem cont = new JMenuItem("Continue");
        newgame.addActionListener(this);        
        pause.addActionListener(this);
        cont.addActionListener(this);
        opt.add(newgame);
        opt.add(pause);
        opt.add(cont);        
        bar.add(opt);
        this.setJMenuBar(bar);
        
        addKeyListener(this);
        setBounds(300,100,270,360); // original size: 270:339
        setVisible(true); 
        setResizable(false);                
    }   
    public void actionPerformed(ActionEvent eve){
        String e = eve.getActionCommand();
        int curDir = game.getSnake().getDirectionH();
        if(e.equals("New Game")){
            Pause();
            game.resetGame();
            Continue();
        }else if(e.equals("Pause")){            
            if( (!paused)&&(!game.hasLost()) ) Pause();  
        }else if(e.equals("Continue")){            
            if( (paused)&&(!game.hasLost()) ) Continue();            
        }
    }    
    public void keyReleased(KeyEvent keyeve){}
    public void keyTyped(KeyEvent keyeve){
        char c = keyeve.getKeyChar();
        if(!canPause) return;
        switch(c){
            case 'p':
            case 'P':                                    
                Pause();
                
        }
    }        
    public void keyPressed(KeyEvent keyeve){
        int a = keyeve.getKeyCode(); 
        int curDir = game.getSnake().getDirectionH(); 
        if(!game.readyToChangeDir) return;
        if(paused) return;
        switch(a){
            case 38:    
                if((curDir!= Snake.UP)&&(curDir!= Snake.DOWN)){
                    game.getSnake().setDirectionH(Snake.UP);  
                    game.getMap().setCell(game.getSnake().getHeadH(),game.getSnake().getHeadW(),Map.BRANCH_UP);
                } 
                break;
            case 39:
                if((curDir!= Snake.RIGHT)&&(curDir!= Snake.LEFT)){
                    game.getSnake().setDirectionH(Snake.RIGHT);
                    game.getMap().setCell(game.getSnake().getHeadH(),game.getSnake().getHeadW(),Map.BRANCH_RIGHT);
                }
                break;
            case 40:
                if((curDir!= Snake.DOWN)&&(curDir!= Snake.UP)){
                    game.getSnake().setDirectionH(Snake.DOWN);
                    game.getMap().setCell(game.getSnake().getHeadH(),game.getSnake().getHeadW(),Map.BRANCH_DOWN);
                } 
                break;
            case 37:
                if((curDir!= Snake.LEFT)&&(curDir!= Snake.RIGHT)){
                    game.getSnake().setDirectionH(Snake.LEFT);
                    game.getMap().setCell(game.getSnake().getHeadH(),game.getSnake().getHeadW(),Map.BRANCH_LEFT);        
                }    
                break;                
        }    
        game.readyToChangeDir = false;
    } 
    public void Continue(){
        game.startMoving();
        paused = false;
    } 
    public void Pause(){
        game.stopMoving();
        paused = true;
    } 
    //public void setScoreboard(String str){this.lab.setText(str);}
    
}
