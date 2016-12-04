import java.awt.event.ItemEvent;
import java.io.FileWriter;
import java.io.PrintWriter;
import java.io.Writer;
import java.net.URLEncoder;
import java.nio.charset.StandardCharsets;
import java.text.DateFormat;
import java.text.Format;
import java.text.SimpleDateFormat;
import java.io.BufferedReader;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.util.*;
import javax.xml.crypto.Data;

import com.sun.org.apache.xerces.internal.impl.xpath.regex.ParseException;

import sun.nio.cs.UnicodeEncoder;
public class Text_Parser {	
	public static void main(String [] Args){
		ArrayList<Pump_data> DataList = new ArrayList<>();
		try{
        BufferedReader br = new BufferedReader(new FileReader("HighFlow.txt"));
        String fileRead = br.readLine();
        while (fileRead != null)
        {
            String[] token = fileRead.split(" ");	
    		float a;
    		int b;
    		String s1,s2;	
    		if(token[0].substring(0, 4).equals("High") ||token[0].substring(0, 3).equals("Low")){		
    		}
    		else{
    			 a = Float.parseFloat(token[2].toString());
    			 b = Integer.parseInt(token[3].toString());
    			 s1 = token[0].toString();
    			 s2 = token[1].toString();
    			 Pump_data data = new Pump_data(s1, s2, a, b);
    			 DataList.add(data);	
    		}
            // read next line before looping
            // if end of file reached 
            fileRead = br.readLine();
        }

        // close file stream
        br.close();
		}
		
		catch (FileNotFoundException fnfe)
        {
            System.out.println("file not found");
        }

        catch (IOException ioe)
        {
            ioe.printStackTrace();
        }
		
//		
	}
}