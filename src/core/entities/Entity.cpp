#include "Entity.h"

bool Entity::operator==(const Entity& other) const
{
    return entity == other.getID() && registry == other.registry;
}

bool Entity::operator!=(const Entity& other) const
{
    return !operator==(other);
}

const entt::entity& Entity::getID() const
{
    return entity;
}
