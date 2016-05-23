#include<sys/types.h>
#include<regex.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<xmlparser.h>

#define MAX_ERROR_MSG 0x1000

/*typedef struct node {
	char nodeName[2000];
	char nodeContent[10000];
	int tagMatch;
	int depth, children;
	struct node *parentNode;
	struct node **childNode;
}xmlNode;
*/

xmlNode* processLine(char *line, xmlNode *head);
xmlNode* createNode();
xmlNode* closeCurrentNode(xmlNode *node, char *text);
int insertChild(xmlNode *head);
int checkRegex(char *line, char *pattern, char *text);
int freeXmlTree(xmlNode *head);
int walkThroughTree(xmlNode *head);

int checkRegex(char *line, char *pattern, char *text) {
	
	regex_t exp;
	int status;
	
	status = regcomp(&exp, pattern, REG_EXTENDED|REG_NEWLINE);
    if (status != 0) {
	char error_message[MAX_ERROR_MSG];
	regerror (status, &exp, error_message, MAX_ERROR_MSG);
    printf ("Regex error compiling '%s': %s\n", pattern, error_message);
    return 1;
    }

	regmatch_t match[2];
	int nomatch = regexec(&exp, line, 2, match, 0);
	if (nomatch) {
		//printf ("No more matches.\n");
		return nomatch;
	}

	int start;
	int finish;
	start = match[1].rm_so;
	finish = match[1].rm_eo;
	//printf ("%.*s \n", (finish - start), p + start);
	strncpy(text, (line + start), (finish-start));
	
	regfree(&exp);
	return 0;
}

xmlNode* processLine(char *line, xmlNode *currentNode){

	const size_t pattern_size = 2000;
    char *pattern = malloc(pattern_size);
	char *text = malloc(pattern_size);
	//xmlNode *node;
	
	//printf("Processing line: %s\n", line);
	//check starting Tag
	memset(pattern, '\0', pattern_size);
	memset(text, '\0', pattern_size);
	strcpy(pattern, "<([a-zA-Z0-9]+)+>");
	checkRegex(line, pattern, text);
	if(strcmp(text,"")!=0) {
		//printf("Found start of Tag: %s\n", text);
		//printf("    Current node: %s\n", currentNode->nodeName);
		//Spawn a new node if start node is found
		//node = createNode();
		currentNode->children++;
		//printf("*** %d\n", currentNode->children); 
		//currentNode->childNode = malloc(sizeof(xmlNode));
		insertChild(currentNode);
		currentNode->childNode[currentNode->children - 1] = createNode();
		currentNode->childNode[currentNode->children - 1]->parentNode = currentNode;
		currentNode->childNode[currentNode->children - 1]->depth = currentNode->depth + 1;
		strcpy(currentNode->childNode[currentNode->children - 1]->nodeName, text);
		currentNode = currentNode->childNode[currentNode->children - 1];
		//printf("    Current node: %s\n", currentNode->nodeName);
	}
	
	//get the attributes
	memset(pattern, '\0', pattern_size);
	memset(text, '\0', pattern_size);
	strcpy(pattern, ">(.*)?</");
	checkRegex(line, pattern, text);
	if(strcmp(text,"")!=0) {
		//printf("Found attributes of Tag: %s\n", text);
		strcpy(currentNode->nodeContent,text);
	}
	
	//check for end Tag
	memset(pattern, '\0', pattern_size);
	memset(text, '\0', pattern_size);
	strcpy(pattern, "</([a-zA-Z0-9]+)>");
	checkRegex(line, pattern, text);
	if(strcmp(text,"")!=0) {
		//printf("Found end of Tag: %s\n", text);
		currentNode = closeCurrentNode(currentNode, text);
	}
	
	free(pattern);
	free(text);
	
	return currentNode;	
}

xmlNode* xmlParser(char *filename){
	FILE *fp;
	const size_t line_size = 2000;
	char *line = malloc(line_size);
	
	fp = fopen(filename, "r+");
	if(fp == NULL) {
		printf("Error: Cannot open the file %s\n", filename);
		return -1;
	}

	xmlNode *head = createNode();
	strcpy(head->nodeName, "MasterNode");
	head->parentNode = NULL;
	head->depth = 0;
	head->children = 0;
	xmlNode *currentNode = head;
	
	while(fgets(line, line_size, fp)!=NULL) {
		//parse the xml file for each line
		currentNode = processLine(line, currentNode);
		memset(line, '\0', line_size);
	}
	
	if(currentNode->parentNode == NULL) {
		printf("SUCCESS: Completed parsing the xml file\n");
	} else {
		printf("FAILURE: Unable to parse the xml file\n");
	}
	
	//printf("-- Traversing the tree\n");
	//walkThroughTree(head);
    //freeXmlTree(head);
 	
	fclose(fp);
	free(line);
	return head;
}

xmlNode* createNode(){
	
	xmlNode *node = (xmlNode *)malloc(sizeof(xmlNode));
	if(node == NULL) {
		printf("Unable to allocate memory for Node\n");
		return NULL;
	}
	node->childNode=NULL;
	node->parentNode=NULL;
	node->children=0;
	node->depth=0;
	return node;
}

int freeXmlTree(xmlNode *head){
	int i;
	
	for(i = 0; i< head->children; i++){
		if(head->childNode[i] != NULL){
			//printf("Freeing the node: %s\n", head->nodeName);
			freeXmlTree(head->childNode[i]);
		}
	}
	//xmlNode *temp;
	//temp = head->parentNode;
	//temp->childNode = NULL;
	//printf("WARNING: Freeing the node: %s\n", head->nodeName);
	free(head);

	return 0;
}

xmlNode* closeCurrentNode(xmlNode *node, char *text){
	if(strcmp(node->nodeName,text)==0) {
		//node->childNode = NULL;
		node->tagMatch = 1;
		node = node->parentNode;
		return node;
	} else {
		printf("The tags do not match: %s %s\n", node->nodeName, text);
		return NULL;
	}
}

int walkThroughTree(xmlNode *head){
	
	unsigned i;
	
	for(i=0; i<head->depth; i++) {
		printf("  ");
	}
	printf("%s = %s, depth= %d children= %d\n",head->nodeName, head->nodeContent, head->depth, head->children);
	
	for(i=0; i<head->children; i++){
		//printf("going in\n");
		walkThroughTree(head->childNode[i]);
		//printf("coming out\n");
	}
	
	return 0;
}

int insertChild(xmlNode *head) {
	//unsigned i;
	head->childNode = (xmlNode** )realloc(head->childNode, (head->children)*sizeof(xmlNode*));
	return 0;
}

int findParentNode(xmlNode *head, char* name, char* text) {
	int i;
	
	if(head == NULL) {
		return 0;
	}
	
	if(parentFound == 1) {
		return 0;
	}
	
	//printf("<%s> -%s-\n", head->nodeName, head->nodeContent);
	
	if ((strcmp(head->nodeContent, text) == 0) && (strcmp(head->nodeName, name) == 0)) {
		parentFound = 1;
		//printf("Found the parent node\n");
		//printf("Parent: <%s>, child: <%s>\n", head->parentNode->nodeName, head->nodeName);
		parentNode = head->parentNode;
		return 0;
	}
	
	for(i=0; (i < head->children) && (parentFound == 0); i++) {
		
		findParentNode(head->childNode[i], name, text);
	}
	
	return 0;
}