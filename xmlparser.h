
typedef struct node {
	char nodeName[2000];
	char nodeContent[10000];
	int tagMatch;
	int depth, children;
	struct node *parentNode;
	struct node **childNode;
}xmlNode;

xmlNode* xmlParser(char *filename);

int findParentNode(xmlNode *head, char* name, char* text);
int freeXmlTree(xmlNode *head);
int walkThroughTree(xmlNode *head);

unsigned parentFound;
xmlNode *parentNode;