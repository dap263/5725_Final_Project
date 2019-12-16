import spotipy 
import spotipy.util as util

redirect_url = 'http://localhost'
username = 'david'
scope = 'streaming'
cli_id = '88a201cbe8254a5ab945a878d579b43d'
cli_sec = 'e7e6b3e17d824f828676e8eb417ddff1'

token = util.prompt_for_user_token(username,scope,client_id=cli_id,client_secret=cli_sec,redirect_uri=redirect_url)

if token:
	sp = spotipy.Spotify(auth=token)
else:
	print "cant get token for", username

spotify = spotipy.Spotify()
results = spotify.search(q='artist:' + 'queen', type='artist')
print results
