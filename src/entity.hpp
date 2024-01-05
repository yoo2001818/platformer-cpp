#ifndef __ENTITY_HPP__
#define __ENTITY_HPP__

class entity;

class component {
  // Component manages the memory inside each entity chunk - the entity's
  // accessor calls the component's methods.
public:
  void *get(const entity &pEntity) const;
  void set(entity &pEntity, void *pValue);
  void remove(entity &pEntity);
};

class entity {};

class entity_chunk {};

class entity_group {};

class entity_handle {};

class entity_query {};

class entity_store {};

#endif // __ENTITY_HPP__
