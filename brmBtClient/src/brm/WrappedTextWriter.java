package brm;

import javax.microedition.lcdui.*;

/**
 * @author http://www.blondmobile.com/2008/02/j2me-ui-text-wrapping-on-canvas_7394.html
 */
public class WrappedTextWriter {
    static Font m_font;
    static String m_txt;
    static int m_length, m_width, m_position, m_start;

    public static int write(Graphics g, String txt, int x, int y,
                             int width, Font font,
                             int alignment ){
        m_font = font;
        m_txt = txt;
        m_length = txt.length();
        m_width = width;
        //reseting
        m_position =0;
        m_start = 0;

        int fontHight = m_font.getHeight() + 1;
        String s;
        g.setFont(m_font);
        while(hasMoreLines()){
        s = nextLine().trim();
        g.drawString(s, x, y, Graphics.TOP|alignment );
        y += fontHight;
        }
        return y;
    }

    private static boolean hasMoreLines(){
        return (m_position<(m_length/*-1*/));
    }


    private static String nextLine(){
         int maxLength = m_txt.length();
         int next = next();
         if(m_start>=maxLength || next>maxLength)
              return null;
         String s =m_txt.substring(m_start, next);
         m_start = next;
         if((m_txt.length()-1>m_start )&& ((m_txt.charAt(m_start)=='\n') ||
            (m_txt.charAt(m_start)==' '))){
              m_position++;
              m_start++;
         }
         return s;
    }


    private static int next(){
         int i=getNextWord(m_position);
         int lastBreak = -1;
         String line;
         line= m_txt.substring(m_position, i);
         int lineWidth = m_font.stringWidth(line);
         while (i<m_length && lineWidth<= m_width){
             if(m_txt.charAt(i)==' ' )
                lastBreak = i;
             else if(m_txt.charAt(i)== '\n'){
                lastBreak =i;
                break;
             }
             if(++i<m_length){
                i= getNextWord(i);
                line = m_txt.substring(m_position, i);
                lineWidth = m_font.stringWidth(line);
             }
         }
         if(i==m_length && lineWidth<= m_width)
            m_position = i;
         else if(lastBreak == m_position)
            m_position++;
         else if(lastBreak < m_position)
            m_position =i;
         else
            m_position = lastBreak;
         return m_position;
      }


    private static int getNextWord(int startIndex){
        int space = m_txt.indexOf(' ', startIndex);
        int newLine = m_txt.indexOf('\n', startIndex);
        if(space ==-1)
           space = m_length;
        if(newLine ==-1)
           newLine = m_length;
        if(space<newLine)
           return space;
        else
          return newLine;
    }

}