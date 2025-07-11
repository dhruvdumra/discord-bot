import discord
from discord.ext import commands
import os
from dotenv import load_dotenv
import asyncio

# Load environment variables
load_dotenv()

# Bot configurations
JOEY_TOKEN = os.getenv('JOEY_TOKEN')
ROSS_TOKEN = os.getenv('ROSS_TOKEN')
# Get owner IDs from environment variables
OWNER_IDS_STR = os.getenv('OWNER_IDS', '847693473112653825')  # Default to your ID
OWNER_IDS = [int(id.strip()) for id in OWNER_IDS_STR.split(',')]

if not JOEY_TOKEN or not ROSS_TOKEN:
    raise ValueError("Both JOEY_TOKEN and ROSS_TOKEN environment variables are required")

# Create intents
intents = discord.Intents.default()
intents.message_content = True
intents.dm_messages = True

# Bot 1 - Joey
joey_bot = commands.Bot(command_prefix='!joey', intents=intents)

# Bot 2 - Ross
ross_bot = commands.Bot(command_prefix='!ross', intents=intents)

# Joey Bot Events
@joey_bot.event
async def on_ready():
    print(f'✅ Joey bot logged in as {joey_bot.user} (ID: {joey_bot.user.id})')
    print('------')

@joey_bot.event
async def on_message(message):
    # Ignore messages from the bot itself
    if message.author == joey_bot.user:
        return
    
    # Only respond to DMs
    if isinstance(message.channel, discord.DMChannel):
        print(f"{message.author} (ID: {message.author.id}) DMed the bot joey: {message.content}")
        try:
            # Send notification to all owners
            for owner_id in OWNER_IDS:
                owner = await joey_bot.fetch_user(owner_id)
                await owner.send(f"📩 *{message.author}* DMed your bot **joey**:\n{message.content}")
        except Exception as e:
            print(f"❌ Could not notify owner from Joey: {e}")
    
    await joey_bot.process_commands(message)

# Ross Bot Events
@ross_bot.event
async def on_ready():
    print(f'✅ Ross bot logged in as {ross_bot.user} (ID: {ross_bot.user.id})')
    print('------')

@ross_bot.event
async def on_message(message):
    # Ignore messages from the bot itself
    if message.author == ross_bot.user:
        return
    
    # Only respond to DMs
    if isinstance(message.channel, discord.DMChannel):
        print(f"{message.author} (ID: {message.author.id}) DMed the bot ross: {message.content}")
        try:
            # Send notification to all owners
            for owner_id in OWNER_IDS:
                owner = await ross_bot.fetch_user(owner_id)
                await owner.send(f"📩 *{message.author}* DMed your bot **ross**:\n{message.content}")
        except Exception as e:
            print(f"❌ Could not notify owner from Ross: {e}")
    
    await ross_bot.process_commands(message)

async def run_both_bots():
    """Run both bots concurrently"""
    print("🚀 Starting both Joey and Ross bots...")
    await asyncio.gather(
        joey_bot.start(JOEY_TOKEN),
        ross_bot.start(ROSS_TOKEN)
    )

if __name__ == "__main__":
    asyncio.run(run_both_bots())
