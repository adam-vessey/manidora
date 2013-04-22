<?php

/**
 * @file
 * Miscellaneous helper functions.
 */

/**
 * Get pids of all sub-collections of supplied collection pid.
 *
 * @param string $collection_pid
 *   pid of ancestor collection
 */
function manidora_get_descendant_collections($collection_pid) {
  $fedora_object = islandora_object_load('islandora:root');

  $dirs_to_process = array($collection_pid);
  $dirs_to_search = array($collection_pid);

  while (count($dirs_to_process) > 0) {
    $constraints = manidora_build_query($dirs_to_process);
    $dirs_to_process = array();
    $ri_query = <<<EOQ
select \$pid
from <#ri>
where (
  ($constraints)
  and  \$pid <fedora-model:state> <fedora-model:Active>
  and (
    \$pid <fedora-model;hasModel> <info:fedora/islandora:collectionCModel>
    or \$pid <fedora-model:hasModel> <info:fedora/islandora:newspaperCModel>
  )
)
EOQ;
    $objects = $fedora_object->repository->ri->itqlQuery($ri_query, 'unlimited');
    foreach ($objects as $object) {
      $dirs_to_search[] = $object['pid']['value'];
      $dirs_to_process[] = $object['pid']['value'];
    }
  }
  return $dirs_to_search;
}

/**
 * Builds itql string from collection pids.
 *
 * @param array $collection_pids
 *   Collection PID.
 *
 * @return string
 *   Query string.
 */
function manidora_build_query($collection_pids) {
  // Map into statements asserting collection  membership.
  $statements = array_map(function ($pid) {
    return "\$pid  <fedora-rels-ext:isMemberOfCollection> <info:fedora/$pid>";
  }, $collection_pids);

  // Implode all statements with "or".
  $disjunction = implode(' or ', $statements);

  return $disjunction;
}
