/* Copyright (c) 2012, STANISLAW ADASZEWSKI
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * Neither the name of STANISLAW ADASZEWSKI nor the
 *    names of its contributors may be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL STANISLAW ADASZEWSKI BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. */

#include "qneport.h"

#include <QFontMetrics>
#include <QGraphicsScene>
#include <graphicelement.h>

#include <QCursor>
#include <QDebug>
#include <QPen>
#include <iostream>
#include <thememanager.h>

#include "qneconnection.h"

QNEPort::QNEPort( QGraphicsItem *parent ) : QGraphicsPathItem( parent ) {
  label = new QGraphicsTextItem( this );
  radius_ = 5;
  margin = 2;

  QPainterPath p;
  p.addPolygon( QRectF( QPointF( -radius_, -radius_ ), QPointF( radius_, radius_ ) ) );
  setPath( p );

  setPen( QPen( Qt::darkRed ) );
  setCurrentBrush( Qt::red );

  setFlag( QGraphicsItem::ItemSendsScenePositionChanges );

  m_portFlags = 0;
  m_value = false;
  m_graphicElement = NULL;
  m_required = true;
  m_defaultValue = -1;
}

void QNEPort::setNEBlock( QNEBlock *b ) {
  m_block = b;
}

void QNEPort::setName( const QString &n ) {
  name = n;
/*  label->setPlainText( n ); */
  setToolTip( n );
}

int QNEPort::radius( ) const {
  return( radius_ );
}

const QList< QNEConnection* > &QNEPort::connections( ) const {
  return( m_connections );
}

void QNEPort::connect( QNEConnection *conn ) {
  if( conn ) {
    if( !m_connections.contains( conn ) ) {
      m_connections.append( conn );
    }
    updateConnections( );
    if( graphicElement( ) ) {

//      graphicElement( )->updatePorts( );
//      if( isOutput( ) ) {
//        graphicElement( )->updateLogic( );
//      }
    }
  }
}

void QNEPort::disconnect( QNEConnection *conn ) {
  m_connections.removeAll( conn );
  if( conn->start( ) == this ) {
    conn->setStart( nullptr );
  }
  if( conn->end( ) == this ) {
    conn->setEnd( nullptr );
  }
  updateConnections( );
}

void QNEPort::setPortFlags( int f ) {
  m_portFlags = f;
  if( m_portFlags & TypePort ) {
    QFont font( scene( )->font( ) );
    font.setItalic( true );
    label->setFont( font );
    setPath( QPainterPath( ) );
  }
  else if( m_portFlags & NamePort ) {
    QFont font( scene( )->font( ) );
    font.setBold( true );
    label->setFont( font );
    setPath( QPainterPath( ) );
  }
}

const QString &QNEPort::portName( ) const {
  return( name );
}

int QNEPort::portFlags( ) const {
  return( m_portFlags );
}

int QNEPort::type( ) const {
  return( Type );
}

quint64 QNEPort::ptr( ) {
  return( m_ptr );
}

void QNEPort::setPtr( quint64 p ) {
  m_ptr = p;
}

bool QNEPort::isConnected( QNEPort *other ) {
  for( QNEConnection *conn : m_connections ) {
    if( ( conn->start( ) == other ) || ( conn->end( ) == other ) ) {
      return( true );
    }
  }
  return( false );
}

void QNEPort::updateConnections( ) {
  for( QNEConnection *conn : m_connections ) {
    conn->updatePosFromPorts( );
    conn->updatePath( );
  }
  if( isValid( ) ) {
    if( ( m_connections.size( ) == 0 ) && !isOutput( ) ) {
      setValue( defaultValue( ) );
    }
  }
  else {
    setValue( -1 );
  }
}

QVariant QNEPort::itemChange( GraphicsItemChange change, const QVariant &value ) {
  if( change == ItemScenePositionHasChanged ) {
    updateConnections( );
  }
  return( value );
}

int QNEPort::index( ) const {
  return( m_index );
}

void QNEPort::setIndex( int index ) {
  m_index = index;
}

QString QNEPort::getName( ) const {
  return( name );
}

int QNEPort::defaultValue( ) const {
  return( m_defaultValue );
}

void QNEPort::setDefaultValue( int defaultValue ) {
  m_defaultValue = defaultValue;
  setValue( defaultValue );
}


QBrush QNEPort::currentBrush( ) const {
  return( _currentBrush );
}

void QNEPort::setCurrentBrush( const QBrush &currentBrush ) {
  _currentBrush = currentBrush;
  if( brush( ).color( ) != Qt::yellow ) {
    setBrush( currentBrush );
  }
}


bool QNEPort::isRequired( ) const {
  return( m_required );
}

void QNEPort::setRequired( bool required ) {
  m_required = required;
  setDefaultValue( -1 * required );
}

char QNEPort::value( ) const {
  return( m_value );
}

GraphicElement* QNEPort::graphicElement( ) const {
  return( m_graphicElement );
}

void QNEPort::setGraphicElement( GraphicElement *graphicElement ) {
  m_graphicElement = graphicElement;
}

void QNEPort::hoverLeave( ) {
  setBrush( currentBrush( ) );
  update( );
}

void QNEPort::hoverEnter( ) {
  setBrush( QBrush( Qt::yellow ) );
  update( );
}

QNEInputPort::QNEInputPort( QGraphicsItem *parent ) : QNEPort( parent ) {
  label->setPos( -radius_ - margin - label->boundingRect( ).width( ), -label->boundingRect( ).height( ) / 2 );
}

QNEInputPort::~QNEInputPort( ) {
  while( !m_connections.isEmpty( ) ) {
    QNEConnection *conn = m_connections.back( );
    m_connections.removeAll( conn );
    conn->setEnd( nullptr );
    delete conn;
  }
}

void QNEInputPort::setValue( char value ) {
  m_value = value;
  if( !isValid( ) ) {
    m_value = -1;
  }
  if( ThemeManager::globalMngr ) {
    const ThemeAttrs &attrs = ThemeManager::globalMngr->getAttrs( );
    if( m_value == -1 ) {
      setPen( attrs.qnePort_invalid_pen );
      setCurrentBrush( attrs.qnePort_invalid_brush );
    }
    else if( m_value == 1 ) {
      setPen( attrs.qnePort_true_pen );
      setCurrentBrush( attrs.qnePort_true_brush );
    }
    else {
      setPen( attrs.qnePort_false_pen );
      setCurrentBrush( attrs.qnePort_false_brush );
    }
  }
}

bool QNEInputPort::isOutput( ) const {
  return( false );
}

bool QNEInputPort::isValid( ) const {
  if( m_connections.isEmpty( ) ) {
    return( !isRequired( ) );
  }
  return( m_connections.size( ) == 1 );
}

void QNEInputPort::updateTheme( ) {

}


QNEOutputPort::QNEOutputPort( QGraphicsItem *parent ) : QNEPort( parent ) {
  label->setPos( radius_ + margin, -label->boundingRect( ).height( ) / 2 );
  updateTheme( );
}

QNEOutputPort::~QNEOutputPort( ) {
  while( !m_connections.isEmpty( ) ) {
    QNEConnection *conn = m_connections.back( );
    m_connections.removeAll( conn );
    conn->setStart( nullptr );
    delete conn;
  }
}

void QNEOutputPort::setValue( char value ) {
  m_value = value;
  for( QNEConnection *conn : connections( ) ) {
    if( value == -1 ) {
      conn->setStatus( QNEConnection::Invalid );
    }
    else if( value == 0 ) {
      conn->setStatus( QNEConnection::Inactive );
    }
    else {
      conn->setStatus( QNEConnection::Active );
    }
    QNEInputPort *port = conn->otherPort( this );
    if( port ) {
      port->setValue( value );
    }
  }
}

bool QNEOutputPort::isOutput( ) const {
  return( true );
}

bool QNEOutputPort::isValid( ) const {
  return( m_value != -1 );
}

void QNEOutputPort::updateTheme( ) {
  if( ThemeManager::globalMngr ) {
    const ThemeAttrs &attrs = ThemeManager::globalMngr->getAttrs( );
    setPen( attrs.qnePort_output_pen );
    setCurrentBrush( attrs.qnePort_output_brush );
  }
}
