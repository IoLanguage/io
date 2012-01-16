/*
 *  IoSceneNode.c
 *  All rights reserved. See _BSDLicense.txt.
 */

#include "IoSceneNode.h"
#include "IoState.h"
#include "IoNumber.h"
#include "IoCFunction.h"
#include "IoNil.h"
#include "IoGL.h"
#include <math.h>

#define DATA(self) ((IoSceneNodeData *)self->data)
static const char *protoId = "SceneNode";

IoTag *IoSceneNode_newTag(void *state)
{
	IoTag *tag = IoTag_newWithName_(protoId);
	IoTag_state_(tag, state);
	IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoSceneNode_rawClone);
	IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoSceneNode_free);
	IoTag_markFunc_(tag, (IoTagMarkFunc *)IoSceneNode_mark);
	return tag;
}

IoSceneNode *IoSceneNode_proto(void *state)
{
	IoObject *self = IoObject_new(state);
	IoObject_tag_(self, IoSceneNode_newTag(state));
	
	self->data = calloc( sizeof(IoSceneNodeData), 1 );
	DATA(self)->position = IoSeq_newVec3f(state, {0, 0, 0} );
	DATA(self)->velocity = IoSeq_newVec3f(state, {0, 0, 0} );
	DATA(self)->subnodes = NULL;
	DATA(self)->event    = NULL;
	
	IoState_registerProtoWithId_(state, self, protoId);
	
	IoObject_addMethod_( self, IOSYMBOL("advance"), IoSceneNode_advance );
	IoObject_addMethod_( self, IOSYMBOL("display"), IoSceneNode_display );
	IoObject_addMethod_( self, IOSYMBOL("setPosition"), IoSceneNode_setPosition );
	IoObject_addMethod_( self, IOSYMBOL("setVelocity"), IoSceneNode_setVelocity );
	IoObject_addMethod_( self, IOSYMBOL("position"), IoSceneNode_position );
	IoObject_addMethod_( self, IOSYMBOL("velocity"), IoSceneNode_velocity );
	IoObject_addMethod_( self, IOSYMBOL("addChildNode"), IoSceneNode_addChildNode );
	IoObject_addMethod_( self, IOSYMBOL("addCallback"), IoSceneNode_addCallback );
	
	return self;
}

IoSceneNode *IoSceneNode_new(void *state)
{
	IoObject *proto = IoState_protoWithId_(state, protoId);
	IoObject *self  = IoSceneNode_rawClone( proto );
	
	return self;
}

IoObject *IoSceneNode_rawClone(IoSceneNode *proto)
{
	IoState *state = IoObject_state(proto);
	IoObject *self = IoObject_rawClonePrimitive( proto );
	/*IoObject_tag_(self, IoObject_tag(proto));*/
	
	self->data = calloc( sizeof(IoSceneNodeData), 1 );
	DATA(self)->position = IoSeq_newVec3f(state, {0, 0, 0} );
	DATA(self)->velocity = IoSeq_newVec3f(state, {0, 0, 0} );
	DATA(self)->subnodes = NULL;
	DATA(self)->event    = DATA(proto)->event;
	
	IoState_addValue_(state, self );
	
	return self;
}

void IoSceneNode_free(IoSceneNode *self) 
{ 
	free( self->data );
}

void IoSceneNode_mark(IoSceneNode *self)
{
	if (DATA(self)->subnodes) { IoObject_makeGrayIfWhite( DATA(self)->subnodes ); }
	if (DATA(self)->position) { IoObject_makeGrayIfWhite( DATA(self)->position ); }
	if (DATA(self)->velocity) { IoObject_makeGrayIfWhite( DATA(self)->velocity ); }
	if (DATA(self)->subnodes) { IoObject_makeGrayIfWhite( DATA(self)->subnodes ); }
}

/* ----------------------------------------------------------- */

IoObject *IoSceneNode_setPosition( IoSceneNode *self, IoObject *locals, IoMessage* msg )
{
	float x = IoMessage_locals_floatArgAt_( msg, locals, 0 );
	float y = IoMessage_locals_floatArgAt_( msg, locals, 1 );
	float z = IoMessage_locals_floatArgAt_( msg, locals, 2 );
	
	IoSeq_setVec3f_( DATA(self)->position, {x, y, z} );
	
	return self;
}

IoObject *IoSceneNode_setVelocity( IoSceneNode *self, IoObject *locals, IoMessage* msg )
{
	float x = IoMessage_locals_floatArgAt_( msg, locals, 0 );
	float y = IoMessage_locals_floatArgAt_( msg, locals, 1 );
	float z = IoMessage_locals_floatArgAt_( msg, locals, 2 );
	
	IoSeq_setVec3f_( DATA(self)->velocity, {x, y, z} );
	
	return self;
}

IoObject* IoSceneNode_position( IoSceneNode* self, IoObject* locals, IoMessage* msg )
{
	return DATA(self)->position;
}

IoObject *IoSceneNode_velocity( IoSceneNode *self, IoObject *locals, IoMessage* msg )
{
	return DATA(self)->velocity;
}

IoObject *IoSceneNode_advance(IoSceneNode *self, IoObject *locals, IoMessage *m)
{
	UArray *position, *velocity;
	
	position = IoSeq_rawUArray( ((IoSceneNodeData *)self->data)->position );
	velocity = IoSeq_rawUArray( ((IoSceneNodeData *)self->data)->velocity );
	
	UArray_add_( position, velocity );
	/*Vector_at_put_( velocity, 1, UArray_rawDoubleAt_( velocity, 1 ) - 0.001 );*/
	
	if( DATA(self)->subnodes )
	{
		List *subnodes = IoList_rawList( DATA(self)->subnodes );
		int i, count   = List_count( subnodes );
		IoSceneNode *node = NULL;
		
		for( i = 0; i < count; i++ ) 
		{
			node = List_at_( subnodes, i );
			
			if( node < 0x1000 ) 
			{
				puts( "Invalid node pointer" );
				return self;
			}
			
			position = IoSeq_rawUArray( DATA(node)->position );
			velocity = IoSeq_rawUArray( DATA(node)->velocity );
			
			UArray_add_( position, velocity );
			UArray_at_put_( velocity, 1, UArray_rawDoubleAt_( velocity, 1 ) - 0.001 );
			
			if( DATA(node)->event )
			{
				IoSceneEvent *event = DATA(node)->event;
				switch( event->var ) 
				{
					case AXIS_Y: switch( event->test ) 
					{
						case LESS_THAN: {
							if( UArray_doubleAt_( position, 1 ) < event->val )
							{
								printf( "event callback\n" );
								IoMessage_locals_performOn_( DATA(node)->event->message, locals, node );
							}
						}; break;
					}; break;
				}
			}
			
			if( DATA(node)->subnodes )
			{
				IoSceneNode_advance( node, locals, m );
			}
		}
	}
	
	return self;
}


IoObject *IoSceneNode_display(IoSceneNode *self, IoObject *locals, IoMessage *m)
{
	Vector *position;
	
	position = IoSeq_rawUArray( DATA(self)->position );
	
	glVertex3d( UArray_rawDoubleAt_( position, 0 ), UArray_rawDoubleAt_( position, 1 ),
			  UArray_rawDoubleAt_( position, 2 ) );
	
	if( DATA(self)->subnodes )
	{
		List *subnodes = IoList_rawList( DATA(self)->subnodes );
		int i, count   = List_count( subnodes );
		IoSceneNode *node = NULL;
		
		for( i = 0; i < count; i++ ) {
			node = List_at_( subnodes, i );
			
			position = IoSeq_rawUArray( DATA(node)->position );
			
			glVertex3d( UArray_rawDoubleAt_( position, 0 ), UArray_rawDoubleAt_( position, 1 ),
					  UArray_rawDoubleAt_( position, 2 ) );
			
			if( DATA(node)->subnodes )
			{
				IoSceneNode_display( node, locals, m );
			}
		}
	}
	
	return self;
}

IoObject *IoSceneNode_addChildNode( IoSceneNode *self, IoObject *locals, IoMessage *msg )
{
	IoObject *node = IoMessage_locals_valueArgAt_( msg, locals, 0 );
	
	if( DATA(self)->subnodes == NULL )
	{
		DATA(self)->subnodes = IoList_new(IOSTATE);
	}
	
	IoList_rawAdd_( DATA(self)->subnodes, node );
	
	return self;
}

IoObject *IoSceneNode_addCallback( IoSceneNode *self, IoObject *locals, IoMessage *msg )
{
	int  var  = IoMessage_locals_intArgAt_( msg, locals, 0 );
	int  test = IoMessage_locals_intArgAt_( msg, locals, 1 );
	int  val  = IoMessage_locals_intArgAt_( msg, locals, 2 );
	IoMessage *body = IoMessage_rawArgAt_( msg, 3 );
	
	IoSceneEvent event = { var, test, val, body };
	
	DATA(self)->event = calloc( sizeof(IoSceneEvent), 1 );
	*DATA(self)->event = event;
	
	return self;
}

