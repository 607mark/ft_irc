#include "commands.hpp"

// Change the signature to use std::shared_ptr<Client>
std::string handlePart(Server *server, const std::vector<std::string> &args, std::shared_ptr<Client> client)
{
    // Check registration
    if (!client->isRegistered())
        return "451 " + client->getNick() + " :You have not registered\r\n";

    if (args.size() < 2)
        return "461 " + client->getNick() + " PART :Not enough parameters\r\n";

    std::string channelName = args[1];
    std::string partMessage = "";

    // Optional part message
    if (args.size() > 2)
    {
        for (size_t i = 2; i < args.size(); ++i)
        {
            if (i > 2)
                partMessage += " ";
            partMessage += args[i];
        }
        if (!partMessage.empty() && partMessage[0] == ':')
            partMessage = partMessage.substr(1);
    }

    auto &channels = server->getChannels();
    auto channelIt = channels.find(channelName);

    // Channel must exist
    if (channelIt == channels.end())
        return "403 " + client->getNick() + " " + channelName + " :No such channel\r\n";

    std::shared_ptr<Channel> channel = channelIt->second;

    // Client must be on the channel to leave it
    if (!channel->hasUser(client)) // Pass shared_ptr
        return "442 " + client->getNick() + " " + channelName + " :You're not on that channel\r\n";

    // Create PART message
    std::string partMsg = ":" + client->getNick() + "!" + client->getUsername() + "@localhost" +
                          " PART " + channelName;
    if (!partMessage.empty())
        partMsg += " :" + partMessage;
    partMsg += "\r\n";

    // Send PART message to all channel members
    for (const auto &member : channel->getUsers())
    {
        send(member->getFd(), partMsg.c_str(), partMsg.length(), 0);
    }

    // Remove user from channel
    channel->removeUser(client); // Pass shared_ptr

    // Remove from operators if they were one
    if (channel->isOperator(client))     // Pass shared_ptr
        channel->removeOperator(client); // Pass shared_ptr

    // If channel is empty, remove it
    if (channel->getUsers().empty())
    {
        channels.erase(channelName);
    }

    return "";
}