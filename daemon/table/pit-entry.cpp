/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2014-2016,  Regents of the University of California,
 *                           Arizona Board of Regents,
 *                           Colorado State University,
 *                           University Pierre & Marie Curie, Sorbonne University,
 *                           Washington University in St. Louis,
 *                           Beijing Institute of Technology,
 *                           The University of Memphis.
 *
 * This file is part of NFD (Named Data Networking Forwarding Daemon).
 * See AUTHORS.md for complete list of NFD authors and contributors.
 *
 * NFD is free software: you can redistribute it and/or modify it under the terms
 * of the GNU General Public License as published by the Free Software Foundation,
 * either version 3 of the License, or (at your option) any later version.
 *
 * NFD is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * NFD, e.g., in COPYING.md file.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "pit-entry.hpp"
#include <algorithm>

namespace nfd {
namespace pit {

Entry::Entry(const Interest& interest)
  : m_interest(interest.shared_from_this())
  , m_nameTreeEntry(nullptr)
{
}

InRecordCollection::iterator
Entry::getInRecord(const Face& face)
{
  return std::find_if(m_inRecords.begin(), m_inRecords.end(),
    [&face] (const InRecord& inRecord) { return &inRecord.getFace() == &face; });
}

InRecordCollection::iterator
Entry::insertOrUpdateInRecord(Face& face, const Interest& interest)
{
  auto it = std::find_if(m_inRecords.begin(), m_inRecords.end(),
    [&face] (const InRecord& inRecord) { return &inRecord.getFace() == &face; });
  if (it == m_inRecords.end()) {
    m_inRecords.emplace_front(face);
    it = m_inRecords.begin();
  }

  it->update(interest);
  return it;
}

void
Entry::deleteInRecord(const Face& face)
{
  auto it = std::find_if(m_inRecords.begin(), m_inRecords.end(),
    [&face] (const InRecord& inRecord) { return &inRecord.getFace() == &face; });
  if (it != m_inRecords.end()) {
    m_inRecords.erase(it);
  }
}

bool
Entry::hasNonExpiredLongLivedInRecord(time::steady_clock::time_point now)
{
  for(InRecordCollection::iterator r = m_inRecords.begin(); r != m_inRecords.end(); r++) {
    if(r->getExpiry() > now && r->getInterest().isLongLived()) {
      return true;
    }
  }
  return false;
}

void
Entry::deleteExpiredOrNonLongLivedInRecords(time::steady_clock::time_point now)
{
  for(InRecordCollection::iterator r = m_inRecords.begin(); r != m_inRecords.end(); r++) {
    if(r->getExpiry() <= now || !r->getInterest().isLongLived()) {
      r = m_inRecords.erase(r);
    }
  }
}

void
Entry::clearInRecords()
{
  m_inRecords.clear();
}

OutRecordCollection::iterator
Entry::getOutRecord(const Face& face)
{
  return std::find_if(m_outRecords.begin(), m_outRecords.end(),
    [&face] (const OutRecord& outRecord) { return &outRecord.getFace() == &face; });
}

OutRecordCollection::iterator
Entry::insertOrUpdateOutRecord(Face& face, const Interest& interest)
{
  auto it = std::find_if(m_outRecords.begin(), m_outRecords.end(),
    [&face] (const OutRecord& outRecord) { return &outRecord.getFace() == &face; });
  if (it == m_outRecords.end()) {
    m_outRecords.emplace_front(face);
    it = m_outRecords.begin();
  }

  it->update(interest);
  return it;
}

void
Entry::deleteOutRecord(const Face& face)
{
  auto it = std::find_if(m_outRecords.begin(), m_outRecords.end(),
    [&face] (const OutRecord& outRecord) { return &outRecord.getFace() == &face; });
  if (it != m_outRecords.end()) {
    m_outRecords.erase(it);
  }
}

} // namespace pit
} // namespace nfd
