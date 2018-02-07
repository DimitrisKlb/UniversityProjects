import java.awt.*;
import java.awt.event.*;
import javax.swing.*;

public class IconPuzzleGame extends java.applet.Applet implements ActionListener, MouseListener{
    int moves = 0;
    boolean won = false;
    
    JPanel gamePane = new JPanel();
    JPanel up = new JPanel();
    JPanel down = new JPanel();
    GamePanel VictoryPane, black;
    GamePanel[][] parts;
    GamePanel[] win;
    
    JButton newBut = new JButton("New Game");
    JLabel showMoves = new JLabel("Moves: " + 0);
    
    java.util.Random rand;

    public void init(){
        BorderLayout border = new BorderLayout();
        GridLayout grid = new GridLayout(3, 3, 1, 1);
        setLayout(border);    
        newBut.addActionListener(this);
        gamePane.setLayout(grid);
        gamePane.setBackground(Color.WHITE);
        rand = new java.util.Random();
        
        parts = new GamePanel[5][5];
        win = new GamePanel[9];     
        
        int i;
        for(i=0;i<8;i++){
            win[i] = new GamePanel("icons/katanas/katana_"+i+".jpg");      
            win[i].addMouseListener(this);          
        }      
        win[8] = new GamePanel("icons/black.jpg");
        black = win[8];
        VictoryPane = new GamePanel("icons/katanas/katana_8.jpg");
              
        partsRandomize();        
        GamePanel nothing = new GamePanel("icons/nothing.jpg");  
		
		int k;
		for(i=0; i<5; i++){
			for(k=0; k<5; k++){
				parts[i][k] = nothing;   
			}
		}		          
        up.add(newBut);
        down.add(showMoves);
        
        add("North", up);
        add("Center", gamePane);
        add("South", down);
        setSize(600, 600);        
    }   


	public void actionPerformed(ActionEvent a){
		moves = 0;
		won = false;
		showMoves.setText("Moves: "+moves);
		gamePane.removeAll();

		partsRandomize();
		validate();
		repaint();
	}   
      
   public void mouseClicked(MouseEvent mEve){
       Object source = mEve.getSource();
       GamePanel clicked = (GamePanel)source;
       
       
       if(mEve.getModifiers()!=MouseEvent.BUTTON1_MASK)
            return;
        
       int xCl=0,yCl=0;
       int X,Y;
        
       for(X=0;X<5;X++){
           for(Y=0;Y<5;Y++){
               if(clicked == parts[Y][X]){
                  xCl=X;
                  yCl=Y;
               }   
           }    
       }  
       
       if(canMove(yCl, xCl) & (!won)){
			moves++;
			showMoves.setText("Moves: "+moves);
					 
			move(yCl,xCl);
			validate();  
			repaint();
             
			if(correctCombo()){
				won = true;

				JOptionPane.showMessageDialog(null,"Well Done!!!","Message",JOptionPane.INFORMATION_MESSAGE);
				gamePane.removeAll();
				for(int i=0;i<8;i++){
					gamePane.add(win[i]);               
				}
				gamePane.add(VictoryPane);
				validate();
				repaint();
			}
                
        }
    }
	
    public void mouseEntered(MouseEvent b){}
    public void mouseExited(MouseEvent b){}
    public void mousePressed(MouseEvent b){}
    public void mouseReleased(MouseEvent b){}
      
    public void partsRandomize(){
        int g1=0, g2=0, g3=0, g4=0;
		int h8;
        
        for(int i=1; i<4;i++){
           for(int k=1; k<4;k++){   
               parts[i][k] = win[h--];
           }    
        }  
        for(int k=0;k<1000;k++){   
            GamePanel temp;
            g1 = rand.nextInt(500)%3 + 1;
            g2 = rand.nextInt(500)%3 + 1;
            g3 = rand.nextInt(500)%3 + 1;       
            g4 = rand.nextInt(500)%3 + 1;
            temp = parts[g1][g2];
            parts[g1][g2] = parts[g3][g4];
            parts[g3][g4] = temp;
        }   
        for(int i=1;i<4;i++){
           for(int k=1;k<4;k++){   
               gamePane.add(parts[i][k]);
           }    
        }   
   }   
   
	public boolean correctCombo(){
	int value = 0;

		for(int i=1;i<4;i++){
			for(int k=1;k<4;k++){   
				if(parts[i][k] != win[value++]){
					return false;                           
				}    
			}    
		} 
		return true;
	}
   
	public boolean canMove(int y,int x){       
		if(parts[y-1][x] == black){ return true;}
		if(parts[y+1][x] == black){ return true;}
		if(parts[y][x-1] == black){ return true;}
		if(parts[y][x+1] == black){ return true;}    
		return false;       
	}  
   
	public void move(int yFrom,int xFrom){
		for(int i=1;i<4;i++){
			for(int k=1;k<4;k++){
				if(parts[i][k] == black){
					parts[i][k] = parts[yFrom][xFrom]; 
				}					
			}    
		}   
		parts[yFrom][xFrom] = black;

		for(int i=1;i<4;i++){
			for(int k=1;k<4;k++){   
				gamePane.add(parts[i][k]);					   
			}    
		}
	}    
       
       
}