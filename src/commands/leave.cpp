#include "commands.hpp"

std::string handlePart(Server *server, const std::vector<std::string> &args, Client &client)
{
    // Check registration
    if (!client.isRegistered())
        return "451 " + client.getNick() + " :You have not registered\r\n";

    if (args.size() < 2)
        return "461 " + client.getNick() + " PART :Not enough parameters\r\n";

    std::string channelName = args[1];
    std::string partMessage = "";

    // part message
    if (args.size() > 2)
    {
        for (size_t i = 2; i < args.size(); ++i)
        {
            if (i > 2)
                partMessage += " ";
            partMessage += args[i];
        }
        // Remove leading colon if present
        if (!partMessage.empty() && partMessage[0] == ':')
            partMessage = partMessage.substr(1);
    }

    auto &channels = server->getChannels();
    auto channelIt = channels.find(channelName);

    // Channel must exist
    if (channelIt == channels.end())
        return "403 " + client.getNick() + " " + channelName + " :No such channel\r\n";

    std::shared_ptr<Channel> channel = channelIt->second;

    // Client must be on the channel to leave it
    if (!channel->hasUser(client))
        return "442 " + client.getNick() + " " + channelName + " :You're not on that channel\r\n";

    // Create PART message
    std::string partMsg = ":" + client.getNick() + "!" + client.getUsername() + "@localhost" +
                          " PART " + channelName;
    if (!partMessage.empty())
        partMsg += " :" + partMessage;
    partMsg += " s\r\n";

    // Send PART message to ALL channel members (including the leaving user)
    for (const Client &member : channel->getUsers())
    {
        send(member.getFd(), partMsg.c_str(), partMsg.length(), 0);
    }

    channel->removeUser(client.getFd());

    // Remove from operators if they were one
    if (channel->isOperator(client))
        channel->removeOperator(client.getFd());

    // If channel is empty, remove it
    // if (channel->getUsers().empty())
    // {
    //     channels.erase(channelName);
    // }

    return "";
}