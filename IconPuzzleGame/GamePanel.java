import javax.swing.*;
import java.awt.*;
import java.awt.image.BufferedImage;
import java.io.*;
import java.util.logging.Level;
import java.util.logging.Logger;
import javax.imageio.*;

public class GamePanel extends JPanel{
    private BufferedImage img;
    
    public GamePanel(String imgDest){
        try{
          img = ImageIO.read(new File(imgDest));  
        }catch(IOException ioe){
            System.out.println("Error: Background image could not be loaded");    
        }         
    }
	
    public void paintComponent(Graphics g){
        g.drawImage(img, 0, 0, getSize().width, getSize().height, null);    
    }  
	
    public void setImage(String imgPath){
        try{
          img = ImageIO.read(new File(imgPath));  
        }catch(IOException ioe){
            System.out.println("Error: Background image could not be loaded");    
        }       
        
    }    
    
}