/*
  ==============================================================================

   This file is part of the JUCE library.
   Copyright (c) 2016 - ROLI Ltd.

   Permission is granted to use this software under the terms of the ISC license
   http://www.isc.org/downloads/software-support-policy/isc-license/

   Permission to use, copy, modify, and/or distribute this software for any
   purpose with or without fee is hereby granted, provided that the above
   copyright notice and this permission notice appear in all copies.

   THE SOFTWARE IS PROVIDED "AS IS" AND ISC DISCLAIMS ALL WARRANTIES WITH REGARD
   TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND
   FITNESS. IN NO EVENT SHALL ISC BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT,
   OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
   USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
   TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE
   OF THIS SOFTWARE.

   -----------------------------------------------------------------------------

   To release a closed-source product which uses other parts of JUCE not
   licensed under the ISC terms, commercial licenses are available: visit
   www.juce.com for more information.

  ==============================================================================
*/

#ifndef JUCE_NOTIFICATIONTYPE_H_INCLUDED
#define JUCE_NOTIFICATIONTYPE_H_INCLUDED

//==============================================================================
/**
    These enums are used in various classes to indicate whether a notification
    event should be sent out.
*/
enum NotificationType
{
    dontSendNotification = 0,   /**< No notification message should be sent. */
    sendNotification = 1,       /**< Requests a notification message, either synchronous or not. */
    sendNotificationSync,       /**< Requests a synchronous notification. */
    sendNotificationAsync,      /**< Requests an asynchronous notification. */
};


#endif   // JUCE_NOTIFICATIONTYPE_H_INCLUDED
