#pragma once

#include <osre/Common/Object.h>

namespace OSRE {

namespace Scene {
    class Node;
}

namespace App {

class AbstractBehaviour;

class Entity : public Common::Object {
public:
    Entity( const String &name );
    virtual ~Entity();
    virtual void setBehaviourControl(AbstractBehaviour *behaviour );
    virtual void setNode( Scene::Node *node );
    virtual bool preprocess();
    virtual bool update( Time dt );
    virtual bool render();
    virtual bool postprocess();

private:
    AbstractBehaviour *m_behaviour;
    Scene::Node *m_node;
};

} // Namespace App
} // Namespace OSRE
