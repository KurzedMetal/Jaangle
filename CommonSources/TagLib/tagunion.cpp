/***************************************************************************
    copyright            : (C) 2002 - 2008 by Scott Wheeler
    email                : wheeler@kde.org
 ***************************************************************************/

/***************************************************************************
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Lesser General Public License version   *
 *   2.1 as published by the Free Software Foundation.                     *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful, but   *
 *   WITHOUT ANY WARRANTY; without even the implied warranty of            *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  *
 *   USA                                                                   *
 *                                                                         *
 *   Alternatively, this file is available under the Mozilla Public        *
 *   License Version 1.1.  You may obtain a copy of the License at         *
 *   http://www.mozilla.org/MPL/                                           *
 ***************************************************************************/

#include "tagunion.h"

using namespace TagLib;

#define stringUnion(method)                                          \
  if(tag(0) && !tag(0)->method().isEmpty())                          \
    return tag(0)->method();                                         \
  if(tag(1) && !tag(1)->method().isEmpty())                          \
    return tag(1)->method();                                         \
  if(tag(2) && !tag(2)->method().isEmpty())                          \
    return tag(2)->method();                                         \
  return String::null                                                \

#define numberUnion(method)                                          \
  if(tag(0) && tag(0)->method() > 0)                                 \
    return tag(0)->method();                                         \
  if(tag(1) && tag(1)->method() > 0)                                 \
    return tag(1)->method();                                         \
  if(tag(2) && tag(2)->method() > 0)                                 \
    return tag(2)->method();                                         \
  return 0

#define setUnion(method, value)                                      \
  if(tag(0))                                                         \
    tag(0)->set##method(value);                                      \
  if(tag(1))                                                         \
    tag(1)->set##method(value);                                      \
  if(tag(2))                                                         \
    tag(2)->set##method(value);                                      \

class TagUnion::TagUnionPrivate
{
public:
  TagUnionPrivate() : tags(3, static_cast<Tag *>(0))
  {

  }

  ~TagUnionPrivate()
  {
    delete tags[0];
    delete tags[1];
    delete tags[2];
  }

  std::vector<Tag *> tags;
};

TagUnion::TagUnion(Tag *first, Tag *second, Tag *third)
{
  d = new TagUnionPrivate;

  d->tags[0] = first;
  d->tags[1] = second;
  d->tags[2] = third;
}

TagUnion::~TagUnion()
{
  delete d;
}

Tag *TagUnion::operator[](int index) const
{
  return tag(index);
}

Tag *TagUnion::tag(int index) const
{
  return d->tags[index];
}

void TagUnion::set(int index, Tag *tag)
{
  delete d->tags[index];
  d->tags[index] = tag;
}

String TagUnion::title() const
{
  stringUnion(title);
}

String TagUnion::artist() const
{
  stringUnion(artist);
}

String TagUnion::album() const
{
  stringUnion(album);
}

String TagUnion::comment() const
{
  stringUnion(comment);
}

String TagUnion::genre() const
{
  stringUnion(genre);
}

TagLib::uint TagUnion::year() const
{
  numberUnion(year);
}

TagLib::uint TagUnion::track() const
{
  numberUnion(track);
}

void TagUnion::setTitle(const String &s)
{
  setUnion(Title, s);
}

void TagUnion::setArtist(const String &s)
{
  setUnion(Artist, s);
}

void TagUnion::setAlbum(const String &s)
{
  setUnion(Album, s);
}

void TagUnion::setComment(const String &s)
{
  setUnion(Comment, s);
}

void TagUnion::setGenre(const String &s)
{
  setUnion(Genre, s);
}

void TagUnion::setYear(uint i)
{
  setUnion(Year, i);
}

void TagUnion::setTrack(uint i)
{
  setUnion(Track, i);
}

bool TagUnion::isEmpty() const
{
  if(d->tags[0] && !d->tags[0]->isEmpty())
    return false;
  if(d->tags[1] && !d->tags[1]->isEmpty())
    return false;
  if(d->tags[2] && !d->tags[2]->isEmpty())
    return false;

  return true;
}

//=== Alex Tags

 String TagUnion::lyrics_AL() const
{
	stringUnion(lyrics_AL);
}
void TagUnion::setLyrics_AL(const String &s)
{
	setUnion(Lyrics_AL, s);
}
uint TagUnion::rating_AL() const
{
	numberUnion(rating_AL);
}
void TagUnion::setRating_AL(uint i)
{
	setUnion(Rating_AL, i);
}
bool TagUnion::picture_AL(String& mime, ByteVector& data, String& description) const
{
	for (int i = 0; i < 3; i++)
	{
		if(tag(i))
		{
			if (tag(i)->picture_AL(mime, data, description))
				return true;
		}
	}
	return false;
}
void TagUnion::setPicture_AL(String& mime, ByteVector& data, String& description)
{
	for (int i = 0; i < 3; i++)
	{
		if(tag(i))
			tag(i)->setPicture_AL(mime, data, description);
	}
}

//=== Album Artist
String TagUnion::albumArtist_AL() const
{
	stringUnion(albumArtist_AL);
}
void TagUnion::setAlbumArtist_AL(const String &s)
{
	setUnion(AlbumArtist_AL, s);
}
//=== Composer
String TagUnion::composer_AL() const
{
	stringUnion(composer_AL);
}
void TagUnion::setComposer_AL(const String &s)
{
	setUnion(Composer_AL, s);
}



//=== Alex Tags END
