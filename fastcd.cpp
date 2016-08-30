
/*
code by phoneli.
email : phone.wc.li@foxmail.com
license : MIT
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <ncurses.h>

#include <errno.h>
#include <vector>
#include <map>
#include <set>
#include <string>
#include <algorithm>

#define STR2UINT(str) strtoul( str , NULL , 10 )

const int baseY = 1;
const int baseX = 1;
int maxRow = 0;
int maxCol = 0;

std::string gStrPath;
std::string gStrIdxFile;

//string---------->
unsigned int PathLen( const char * line )
{
	if( NULL == line ) return 0;
	unsigned int iLen = strlen(line);
	for( unsigned int i = 0 ; i < iLen ; ++i )
	{
		if( ' ' == line[i] || '\n' == line[i] )
			return i;
	}
	return iLen-1;
}

void TrimString(std::string& str)
{
	str.erase(0,str.find_first_not_of(" \t"));
	str.erase(str.find_last_not_of(" \t")+1);
}

void TrimHeadString(std::string& str)
{
	size_t pos = str.find_first_of('"');
	if( pos != std::string::npos )
		str.erase(0, pos+1);
	pos = str.find_last_of('"');
	if( pos != std::string::npos )	
		str.erase( pos );
}

void TrimForLastKey(std::string & str)
{
	size_t pos = str.rfind( "/");
	if( std::string::npos != pos )
		str.erase(0, pos+1);
}

bool bIsOutWordRange( const char s )
{
	int v = (int)s;
	if( 48 <= v && v <= 57 ) return false;
	if( 65 <= v && v <= 90 ) return false;
	if( 97 <= v && v <= 122 ) return false;
	return true;
}

bool bIsMyWord( const std::string & strKey , std::string & strLine )
{
	size_t pos = strLine.find( strKey );
	if( pos != std::string::npos )
	{
		if( pos != 0 && false == bIsOutWordRange( strLine[ pos - 1 ] ) )
		{
			return false;
		}
		if( ( pos + strKey.size() ) != strLine.size() &&
			false == bIsOutWordRange( strLine[ pos + strKey.size() ] ) )
		{
			return false;
		}	 
		return true;
	}
	return false;
} 
//<==========string

void func_flash(const char *dir, int depth)
{
    DIR *dp;
    struct dirent *entry;
    if((dp = opendir(dir)) == NULL) {
		//printf("ERR : depth[%d] : %s\n" , depth , dir );
		printf("%s\n" , dir );//May Be Link
		return;
    }
	printf("%s [path]\n" , dir );

	std::string strCurPath = gStrPath;

    while( NULL != (entry = readdir(dp)) )
	{
		if( DT_DIR == entry->d_type || DT_LNK == entry->d_type )
		{
			if( depth >= 3 && DT_DIR != entry->d_type )
			{
				printf("%s/%s\n", strCurPath.c_str() , entry->d_name );
				continue;
			}
            //ignore .
            if(strncmp( ".", entry->d_name , 1) == 0 )
			{ continue; }

			gStrPath += "/";
			gStrPath += entry->d_name;
            //recurse
            func_flash( gStrPath.c_str() ,depth+1);
			gStrPath = strCurPath;
        } else if ( DT_REG == entry->d_type ) {
			printf("%s/%s\n", strCurPath.c_str() , entry->d_name );
		}
    }
    closedir(dp);
}

void func_list( const unsigned int iLines ,
				std::vector<std::string> * strLines = NULL ,
				std::map<std::string , std::vector<std::string> > * strFileMap = NULL )
{
	FILE * fIdxFile = fopen( gStrIdxFile.c_str() , "r" );
	if( NULL == fIdxFile ) { return ; }

	char sLine[1024] = {0};

	unsigned int iCount = 1;
	std::string strLine;
	std::vector<std::string> * strFileMapItem = NULL;

	for( ; NULL != fgets( sLine , sizeof(sLine) , fIdxFile ) && (iCount <= iLines) ; )
	{
		if( NULL != strLines )
		{
			sLine[ strlen(sLine) - 1 ] = '\0';
			strLines->push_back( sLine );
		} else if ( NULL != strFileMap )
		{
			sLine[ strlen(sLine) - 1 ] = '\0';
			strLine = sLine;
			if ( std::string::npos == strLine.find("[path]") )
			{
				TrimForLastKey( strLine );
				strFileMapItem = &( (*strFileMap)[ strLine ] );
				strFileMapItem->push_back( sLine );
			}	
		} else
		{
			printf("\e[0;33m %.*s (%u)\n\e[0m" , (int)strlen(sLine)-1 , sLine , iCount );
		} 
		iCount++;
	}
	fclose( fIdxFile );
	return ;
}

void func_p( const unsigned int iIndex )
{
	FILE * fIdxFile = fopen( gStrIdxFile.c_str() , "r" );
	if( NULL == fIdxFile ) { return ; }

	unsigned int iCount = 1;
	char sLine[1024] = {0};
	for( ; NULL != fgets( sLine , sizeof(sLine) , fIdxFile ) ; )
	{
		if( iIndex == iCount )
		{
			printf("%.*s\n" , PathLen(sLine) , sLine );
			break;
		}
		iCount++; 
	}
	fclose( fIdxFile );
	return ;
}

void func_search_file( std::string & strOutRes )
{
	initscr();
	raw();
	keypad(stdscr, TRUE);

	start_color();
	init_pair(1, COLOR_MAGENTA , COLOR_BLACK);
    init_pair(2, COLOR_YELLOW, COLOR_BLACK);
	
	getmaxyx(stdscr, maxRow , maxCol );

	int y = 0 , x = 0;
	std::string strUnder( maxCol , '_' );
	std::vector< std::string > strKeys;
	std::vector< std::string > strLines;
	char sKey[128];
	while( true )
	{
		move( baseY , baseX );
		
		getstr( sKey );
		if( 0 == strlen(sKey) )
		{ continue;	}
		if( strncmp( sKey , ":" , 1 ) == 0 && strlen(sKey) >= 2 )
		{
			if( strlen(sKey) == 2 && ( sKey[1] == 'r' || sKey[1] == 'R' ) )
			{
				strKeys.clear();strLines.clear();
				clear();
				continue;
			} else if( strlen(sKey) == 2 && ( sKey[1] == 'q' || sKey[1] == 'Q' ) )
			{
				strOutRes = "./ [path]";
				break;
			}
			const unsigned int iLine = strtoul( sKey+1 , NULL , 10 );
			if( iLine >= strLines.size() )
			{ continue; }
			//if( std::string::npos == strLines.at(iLine).find("[path]") )
			//{ continue; }
			strOutRes = strLines.at(iLine);	
			break;
		} 

		if( 0 == strKeys.size() )
		{ func_list( (unsigned int)-1 , &strLines , NULL ); }
		strKeys.push_back( sKey );
		
		{//find
			std::string strKey = sKey;
			
			bool reverse = false;
			bool casecmp = false;
			if( strncasecmp( sKey , "-v " , 3 ) == 0 && strlen(sKey) >= 4 )
			{ 
				strKey = sKey+3;
				reverse = true; 
			} else if( strncasecmp( sKey , "-i " , 3 ) == 0 && strlen(sKey) >= 4 )
			{ 
				strKey = sKey+3;
				casecmp = true;
				transform(strKey.begin(), strKey.end(), strKey.begin(), ::tolower);
			} 
			
			std::vector< std::string > strFindRes;
			for( unsigned int i = 0 ; i < strLines.size() ; ++ i )
			{
				if( true == reverse )
				{
					if( std::string::npos == strLines.at(i).find( strKey ) )
					{ strFindRes.push_back( strLines.at(i) ); }
				} else if ( true == casecmp ) 
				{
					std::string strTmpKey = strLines.at(i);
					transform( strLines.at(i).begin(), strLines.at(i).end(), strTmpKey.begin(), ::tolower);
					if( std::string::npos != strTmpKey.find( strKey ) )
					{ strFindRes.push_back( strLines.at(i) ); }
				} else 
				{
					if( std::string::npos != strLines.at(i).find( strKey ) )
					{ strFindRes.push_back( strLines.at(i) ); }
				}
			}
			strLines.swap( strFindRes );	
		}

		clear();
		move( baseY+1 , baseX );	
	
		attron( A_BOLD | COLOR_PAIR(1) );
		for( unsigned int i = 0 ; i < strKeys.size() ; ++i )
			printw("%s " , strKeys.at(i).c_str() );
		printw("\n%.*s\n" , maxCol , strUnder.c_str() );
		attroff( A_BOLD | COLOR_PAIR(1) );
		
		attron(COLOR_PAIR(2));
		getyx( stdscr , y , x );
		for( int i = 0 ; i+y < maxRow && i < (int)strLines.size() ; ++i )
		{
			printw( "  %s [%d]\n" , strLines.at(i).c_str() , i );
		}
		attroff(COLOR_PAIR(2));
	
		refresh();			
	}
	
	endwin();
	return ;
}

int func_search_define( const char * sKey , const char * sInFile , const unsigned int iSample , 
						const int iMaxDepth , const char * sEndA , const char * sEndB )
{
	if( NULL == sInFile ) return 0;
	
	const std::string strKey = sKey;
	std::string strInFile = sInFile;
	const char * sIncludeCmd = "#include";
	const char * sImportCmd = "import";
 
	std::map<std::string , std::vector<std::string> > strAllFileMap;//for get file-full-path
	func_list( (unsigned int)-1 , NULL , &strAllFileMap );

	std::set< std::string > oFileSet;			//for de-weight
	std::map< std::string * , int > oFileMap;   //for de-weight
	oFileMap.insert( std::map< std::string * ,int >::value_type(&strInFile,0) );	
	while( true )
	{
		const char * sFileName = NULL;
		int iDepth = 0;
		std::map<std::string *, int >::iterator iter;
		for( iter = oFileMap.begin() ; iter != oFileMap.end() ; ++iter )
		{
			if( -1 != iter->second )
			{
				sFileName = iter->first->c_str();
				iDepth = iter->second;
				iter->second = -1;
				break;
			}
		}	
		if( NULL == sFileName )
		{ break; }

		std::string strNewFileName = "";
		if( sEndA != NULL && sEndB != NULL )
		{
			strNewFileName = sFileName;
			size_t iTmp = strNewFileName.rfind( sEndA );
			if( std::string::npos != iTmp )
			{
				strNewFileName.replace( iTmp , strlen(sEndA) , sEndB );
				if( access( strNewFileName.c_str() , 0 ) == 0 )
				{ sFileName = strNewFileName.c_str(); }
			}
		}

		FILE * fFile = fopen( sFileName , "r" );		
		if( NULL == fFile ) 
		{
			printf( "\n-->iDepth[%d] sFileName[%s] not exist\n" , iDepth , sFileName ); 
			continue ; 
		}
	
		unsigned int iLine = 0;
		std::string strTmpLine;
		char sLine[1024] = {0};
		bool bFindKey = false;
		//printf("\n--> %s [file-depth:%d]\n" , sFileName , iDepth ) ;
		for( unsigned int iPrint = 0 ; NULL != fgets( sLine , sizeof(sLine) , fFile ) && iPrint < iSample ; iLine++)
		{
			strTmpLine = sLine;
			TrimString( strTmpLine );
			if( strncmp( sIncludeCmd , strTmpLine.c_str() , 8 ) == 0 ||
				strncmp( sImportCmd , strTmpLine.c_str() , 6 ) == 0 ) 
			{
				if( iDepth < iMaxDepth && std::string::npos != strTmpLine.find( "\"" ) )
				{
					TrimHeadString( strTmpLine );
					TrimForLastKey( strTmpLine );
					if( oFileSet.end() == oFileSet.find( strTmpLine ) )
					{
						//printf("search\n");
						oFileSet.insert( strTmpLine );
						if( strAllFileMap.end() != strAllFileMap.find( strTmpLine ) )
						{
							std::vector< std::string > * strRes;
							strRes = &strAllFileMap[ strTmpLine ];
							for( unsigned int i = 0 ; i < strRes->size() ; ++i )
							{//insert file into map
								oFileMap.insert( std::map< std::string * ,int>::value_type(
												&(strRes->at(i)) , iDepth+1) );	
							}
						}
					} else {
						//printf("save search!!!\n");
					}	
				}
			} else if( true == bIsMyWord( strKey , strTmpLine ) )
			{
				iPrint ++;
				if( false == bFindKey )
				{ printf("\n--> %s [file-depth:%d]\n" , sFileName , iDepth ) ; bFindKey = true; }
				printf("  %.*s [%u]\n" , (int)strTmpLine.size()-1 , strTmpLine.c_str() , iLine );
			}
		}
		fclose( fFile );
	}
	printf("\n");
	return 0;	
}

int main(int argc, char* argv[])
{

    if ( false )
	{
help:
		printf( "\n" );
		printf( "\e[0;33m usage %s [ l | p | flash | index | $key ]\n\e[0m" , argv[0] );
		printf( "\e[0;33m l $line(d:line=10) : list\n\e[0m" );
		printf( "\e[0;33m p $index : print the path of index\n\e[0m" );
		printf( "\e[0;33m flash $path(d:path=$HOME) : refresh file index \n\e[0m" );
		printf( "\e[0;33m index : fast cd to $index path\n\e[0m" );
		printf( "\e[0;33m key : will search $key and cd path if only one matched\n\e[0m" );
		printf( "\e[0;33m search_define key top-file sample-line max-depth a->b \n\e[0m" ); 
		printf( "\e[0;33m magic : [-v reverse] [-i casecmp] [-r restart] [:index] \n\e[0m" );
		printf( "\n" );
    	return 0;
	}
	
	gStrPath = getenv("HOME");
	gStrIdxFile = gStrPath;
	gStrIdxFile += "/.fastcd"; 	

	const char * sFunc = argv[1];
	
	if( argc == 1 )
	{
		std::string strOutRes;
		func_search_file( strOutRes );
		std::string strCmd = "echo \"";
		strCmd += strOutRes;
		strCmd += "\" > ~/.fastcd.pwd";
		system( strCmd.c_str() );	
		return 0;
	}

	if( strcmp( sFunc , "l" ) == 0 || 
		strcmp( sFunc , "ls" ) == 0 || 
		strcmp( sFunc , "list" ) == 0 )
	{
		unsigned int iLines = 10;
		if( 3 == argc ) { iLines = STR2UINT(argv[2]); }
		func_list( iLines );
	} 
	else if ( strcmp( sFunc , "p" ) == 0 )
	{
		if( argc != 3 ) goto help;
		unsigned int iIndex = STR2UINT(argv[2]);
		func_p( iIndex );
	} 
	else if ( strcmp( sFunc , "flash" ) == 0 )
	{
		if( 3 == argc )
			gStrPath = argv[2];
    	func_flash( gStrPath.c_str() ,0);
	}
	else if ( strcmp( sFunc , "search_define" ) == 0 && ( 6 == argc || 8 == argc ) )
	{
		if( argc == 8 )
			func_search_define( argv[2] , argv[3] , STR2UINT(argv[4]) , STR2UINT(argv[5]) , argv[6] , argv[7] );	
		else
			func_search_define( argv[2] , argv[3] , STR2UINT(argv[4]) , STR2UINT(argv[5]) , NULL , NULL );	
	} 
	else if ( strcmp( sFunc , "debug-list-map" ) == 0 && 3 == argc )
	{
		std::map<std::string , std::vector<std::string> > strFileMap4test;
		func_list( (unsigned int)-1 , NULL , &strFileMap4test );

		std::string strKey = argv[2];
		
		std::vector<std::string> * strRes = &(strFileMap4test[ strKey ]);
		printf("strKey[%s] size[%zu] strRes[%zu]\n" , 
				strKey.c_str() , strFileMap4test.size() , strRes->size() );
		for( unsigned int i = 0 ; i < strRes->size() ; ++i )
		{
			printf("[%u] [%s]\n" , i , strRes->at(i).c_str() );
		}
	}  
	else if ( strcmp( sFunc , "debug-list" ) == 0 && 2 == argc )
	{
		std::vector<std::string> strLines;
		func_list( (unsigned int)-1 , &strLines , NULL );
		for( unsigned int i = 0 ; i  < strLines.size() ; ++i )
		{
			printf("[%u] [%s]\n" , i , strLines.at(i).c_str() );
		}	
	}  
	else 
	{
		goto help;
	} 

    return 0;
}

