/*-----------------------------------------------------------------------------------------------
The MIT License (MIT)

Copyright (c) 2015-2021 OSRE ( Open Source Render Engine ) by Kim Kulling

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
-----------------------------------------------------------------------------------------------*/
#include "osre_testcommon.h"

#include <osre/RenderBackend/RenderCommon.h>
#include <osre/App/Node.h>
#include <osre/App/Component.h>
#include <osre/Common/Ids.h>

namespace OSRE {
namespace UnitTest {

using namespace ::OSRE::App;

class NodeTest : public ::testing::Test {
protected:
    Common::Ids *m_ids;
    std::vector<Node*> m_nodes;

    virtual void SetUp() {
        m_ids = new Common::Ids( 0 );
    }

    virtual void TearDown() {
        delete m_ids;
        m_ids = nullptr;

        for ( auto current : m_nodes ) {
            current->release();
        }
        m_nodes.resize( 0 );
    }

    Node *createNode( const String &name, Common::Ids &ids, Node *parent ) {
        Node *n( new Node( name, ids, parent ) );
        addNodeForRelease( n );
        
        return n;
    }

    void addNodeForRelease( Node *node ) {
        m_nodes.push_back( node );
    }
};

TEST_F( NodeTest, createTest ) {
    bool ok( true );
    try {
        Node *myNode_transform_render = createNode( "testnode1", *m_ids, nullptr );
        EXPECT_NE( nullptr, myNode_transform_render );
    } catch ( ... ) {
        ok = false;
    }
    EXPECT_TRUE( ok );
}

TEST_F( NodeTest, accessChilds ) {
    Node *parent = createNode( "parent", *m_ids, nullptr );
    Node *myNode1 = createNode( "testnode1", *m_ids, parent );
    Node *myNode2 = createNode( "testnode2", *m_ids, parent );

    EXPECT_EQ( 2u, parent->getNumChildren() );
    EXPECT_TRUE( nullptr != myNode1->getParent() );
    EXPECT_TRUE( nullptr != myNode2->getParent() );
    EXPECT_TRUE( nullptr == parent->getParent() );

    Node *found( nullptr );
    found = parent->findChild( "testnode1" );
    EXPECT_TRUE( nullptr != found );

    found = parent->findChild( "testnode3" );
    EXPECT_TRUE( nullptr == found );

    bool ok( true );
    ok = parent->removeChild( "testnode1", Node::TraverseMode::FlatMode );
    EXPECT_TRUE( ok );

    ok = parent->removeChild( "testnode1", Node::TraverseMode::FlatMode );
    EXPECT_FALSE( ok );
}

TEST_F( NodeTest, activeTest ) {
    Node *myNode = createNode( "parent", *m_ids, nullptr );
    EXPECT_TRUE( myNode->isActive() );

    myNode->setActive( false );
    EXPECT_FALSE( myNode->isActive() );
}

TEST_F( NodeTest, onUpdateTest ) {
    Node *myNode = createNode( "parent", *m_ids, nullptr );
    EXPECT_NE(nullptr, myNode);
}

} // Namespace UnitTest
} // Namespace OSRE
