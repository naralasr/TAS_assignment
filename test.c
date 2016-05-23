#include<stdio.h>
#include<stdlib.h>
#include<xmlparser.h>

/*
  This script is to test the xmlparser functionality. 
  The script expects the user input to path of the xml file to be processed.
*/

int main() {
	char filename[200];
	xmlNode *xmlHead;
	
	printf("Please enter the xml file \n");
	scanf("%s", filename);
	xmlHead = (xmlNode *)malloc(sizeof(xmlNode));
	if(xmlHead == NULL){
		printf("Error: Unable to allocate memory for xml tree\n");
		return 1;
	}
	xmlHead = xmlParser(filename);
	//walkThroughTree(xmlHead);
	
	printf("Trying to find the parent of TITLE = Eros\n");
	parentNode = (xmlNode *)malloc(sizeof(xmlNode));
	char nodeName[10] = "TITLE";
	char nodeText[10] = "Eros";
	parentFound = 0;
	xmlNode *head = xmlHead;
	
	/*The find parent function is handicapped as it is an exact string match and not a match by regex*/
	findParentNode(head, nodeName, nodeText);
	printf("Traversing the parent Node\n");
	walkThroughTree(parentNode);
	freeXmlTree(xmlHead);
	
	return 0;
}
