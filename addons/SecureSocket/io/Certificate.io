Certificate do(
	nil
	//stil need to bind int	X509_certificate_type(X509 *x,EVP_PKEY *pubkey);
)

//let's avoid having CertificateName, CertificateAttribute(these live in they pkey), and CertificateExtension objects if possible - let Certificate's API handle that
//also consider certificate chains - 
/*
OBJ_create() should be bound for custom attributes and names and extensions, probably
see openssl ca's configuration option oid_file

X509:
#define		X509_extract_key(x)	X509_get_pubkey(x) //EVP_PKEY - see EVP_PKEY section
int		X509_certificate_type(X509 *x,EVP_PKEY *pubkey);

EVP_PKEY:

int EVP_PKEY_get_attr_count(const EVP_PKEY *key);
X509_ATTRIBUTE *EVP_PKEY_get_attr(const EVP_PKEY *key, int loc);

X509_ATTRIBUTE: (key : attribute);  attribute is a list which forwards to its first element 

int X509_ATTRIBUTE_count(X509_ATTRIBUTE *attr); //multivalued attributes exist
ASN1_OBJECT *X509_ATTRIBUTE_get0_object(X509_ATTRIBUTE *attr); //key
void *X509_ATTRIBUTE_get0_data(X509_ATTRIBUTE *attr, int idx, int atrtype, void *data); //value at index, atrtype is probably an ASN1_TYPE's ->type
ASN1_TYPE *X509_ATTRIBUTE_get0_type(X509_ATTRIBUTE *attr, int idx); //type for value at index

ASN1:

long ASN1_INTEGER_get(ASN1_INTEGER *a);
unsigned char * ASN1_STRING_data(ASN1_STRING *x); //probably works for OCTET_STRING too
//also consider booleans and times and dates


*/