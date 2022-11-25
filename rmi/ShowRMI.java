import javax.naming.*;

public class ShowRMI
{
  public static void main( String[] argv ) throws Exception
  { 
     Context namingContext = new InitialContext();
     NamingEnumeration< NameClassPair > lista = 
       namingContext.list( "rmi://localhost/" );
       
     while (lista.hasMore() ) {
        NameClassPair ncp = lista.next();
        System.out.println( ncp.getName() + "->" + ncp.getClassName() );
     }
  }
}
