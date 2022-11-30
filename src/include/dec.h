#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>
#include <unordered_map>
#include <vector>

namespace buzzdb
{
   namespace dec
   {

      class Hasher{
         public:
         template <typename T>
         uint64_t operator()(const T& ce) const {
            return ce.get_hash();
         }
      };

      /// @brief Represents an uncompressed entry
      /// @tparam T
      template <typename T>
      class Entry
      {
      private:
         T value;

      public:
         Entry(T value) : value{value}
         {
         }

         T get_value()
         {
            return value;
         }

         uint64_t get_hash() const
         {
            return std::hash<T>{}(value);
         }

         bool operator==(const Entry<T> other) const {
            return value == other.value;
         }

         // template <typename E>
         // class Hasher
         // {
         // public:
         //    size_t operator()(const Entry<E> e) const
         //    {
         //       return std::hash<E>{}(e.get_value());
         //    }
         // };
      };

      /// @brief Represents a compressed entry
      class CompressedEntry
      {
      private:
         uint32_t value;

      public:
         CompressedEntry(uint32_t val) : value{val}
         {
         }

         uint32_t get_value()
         {
            return value;
         }

         uint64_t get_hash() const
         {
            return std::hash<uint32_t>{}(value);
         }

         friend bool operator==(const CompressedEntry c1, const CompressedEntry c2);

         // class Hasher
         // {
         //    uint64_t operator()(const CompressedEntry &ce) const
         //    {
         //       return ce.get_hash();
         //    }
         // };
      };

      /// @brief Dictionary data structure for Dictionary Encoding Compression Scheme
      /// - All-at-once encoding
      /// - Uses table-level encoding (all tuples at once)
      /// - Implemented using 2 hash tables for two-way encoding and decoding operations
      /// - Does not support range queries
      /// - Does not preserve order
      /// - Encodes entries to a uint_32t
      /// @tparam E - type of the uncompressed entries
      template <typename E>
      class Dictionary
      {
      public:
         Dictionary(std::vector<Entry<E>> entries): counter{0}{
            for(auto e:entries){
               if(encode_map.find(e.get_value())==encode_map.end()){
                     counter++;
                     encode_map.insert({e.get_value(),counter});
                     decode_map.insert({counter,e.get_value()});
               }
            }
         }
         CompressedEntry compress(Entry<E> e){
            return encode_map.at(e);
         }
         Entry<E> decompress(CompressedEntry ce){
            return decode_map.at(ce);
         }
         size_t size() const
         {
            return counter;
         }
         ~Dictionary()
         {
         }

      private:
         size_t counter;
         std::unordered_map<Entry<E>, CompressedEntry, Hasher> encode_map;
         std::unordered_map<CompressedEntry, Entry<E>, Hasher> decode_map;
         // Dictionary(): counter{0}
         // {
         // }
      };

   } // namespace dec
} // namespace buzzdb