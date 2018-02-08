public class Snake{
    private int headH,headW,tailH,tailW;
    private int initHeadH,initHeadW,initTailH,initTailW;
    private int length;
    private int directionH,directionT;
    
    static final int UP = 6;
    static final int RIGHT = 4;
    static final int DOWN = -6;
    static final int LEFT = -4;
    
    public Snake(int hh,int hw,int th,int tw,int dir){
        initHeadH = headH = hh;
        initHeadW = headW = hw;
        initTailH = tailH = th;
        initTailW = tailW = tw;
        length = hw - tw;
        directionH = directionT = dir; 
    } 
    public void resetPosition(){
        headH = initHeadH;
        headW = initHeadW;
        tailH = initTailH;
        tailW = initTailW;
        length = headW - tailW;
        directionH = directionT = Snake.RIGHT;
    }
    public int getHeadH(){return headH;}
    public int getHeadW(){return headW;}
    public int getTailH(){return tailH;}
    public int getTailW(){return tailW;}
    public int getLength(){return length;}
    public int getDirectionH(){return directionH;}
    public int getDirectionT(){return directionT;}
    
    public void setHeadH(int a){this.headH = a;}
    public void setHeadW(int a){this.headW = a;}
    public void setTailH(int a){this.tailH = a;}
    public void setTailW(int a){this.tailW = a;}
    public void setLength(int length){this.length = length;}
    public void setDirectionH(int directionH){this.directionH = directionH;}
    public void setDirectionT(int directionT){this.directionT = directionT;}
    
}
