
//
// Author: Christian Schulz <christian.schulz@kit.edu>
// 

#ifndef HMAP_WRAPPER_RQFK3ARC
#define HMAP_WRAPPER_RQFK3ARC

#include "data_structure/linear_probing_hashmap.h"

template <typename T>
class hmap_wrapper {
        public:

                hmap_wrapper(PartitionConfig & config) {
                        m_config = config;
                };

                virtual ~hmap_wrapper() {};

                void init( NodeID max_fill_count );
                void clear();
                NodeWeight & operator[](NodeID node);

private:
                T mapping_type;
                PartitionConfig m_config;
};

template <>
class hmap_wrapper < linear_probing_hashmap > {
        public:

                hmap_wrapper(PartitionConfig & config) {
                        m_config = config;
                };

                virtual ~hmap_wrapper() {};

                void init(NodeID max_fill_count )  {mapping_type.init(max_fill_count, m_config.ht_fill_factor);};
                void clear() { mapping_type.clear(); };
                NodeWeight & operator[](NodeID node) {return mapping_type[node];};

        private:
                linear_probing_hashmap mapping_type;
                PartitionConfig m_config;
};

template <>
class hmap_wrapper <std::unordered_map<NodeID, NodeWeight> > {
        public:

                hmap_wrapper(PartitionConfig & config) {
                        m_config = config;
                };

                virtual ~hmap_wrapper() {};

                void init( NodeID max_fill_count )  {};
                void clear() { mapping_type.clear(); };
                NodeWeight & operator[](NodeID node) {return mapping_type[node];};

        private:
                std::unordered_map<NodeID, NodeWeight> mapping_type;
                PartitionConfig m_config;
};

template <>
class hmap_wrapper <std::vector<NodeWeight> > {
        public:

                hmap_wrapper(PartitionConfig & config) {
                        m_config = config;
                };

                virtual ~hmap_wrapper() {};

                void init( NodeID max_fill )  {
                        mapping_type.resize(m_config.k);
                        for( unsigned k = 0; k < m_config.k; k++) {
                                mapping_type[k] = 0;
                        }

                };

                void clear() { 
                        for( unsigned k = 0; k < m_config.k; k++) {
                                mapping_type[k] = 0;
                        }
                };

                NodeWeight & operator[](NodeID node) {return mapping_type[node];};

         private:
                std::vector<NodeWeight> mapping_type;
                PartitionConfig m_config;
};



#endif /* end of include guard: HMAP_WRAPPER_RQFK3ARC */
