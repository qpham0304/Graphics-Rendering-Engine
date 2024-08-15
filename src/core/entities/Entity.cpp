#include "Entity.h"

bool Entity::operator==(const Entity& other) const
{
    return entity == (entt::entity) other.getID() && registry == other.registry;
}

bool Entity::operator!=(const Entity& other) const
{
    return !operator==(other);
}

const uint32_t& Entity::getID() const
{
    return (uint32_t)entity;
}
